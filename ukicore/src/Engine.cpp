/*
 * Manager.cpp
 *
 *  Created on: Jan 19, 2012
 *      Author: nhsan
 */

#include "Engine.h"
#include "commons/SocketUtil.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/epoll.h>
#include <boost/lexical_cast.hpp>
#include <glog/logging.h>
#include "CoreService.h"
#include <boost/make_shared.hpp>

namespace ukicore
{

Engine::Engine()
{
	running = false;
	msg_current_id = 2;

	connections.set_empty_key(-1);
	connections.set_deleted_key(-2);

	contexts.set_empty_key(0);
	contexts.set_deleted_key(1);

	fds.set_empty_key(-1);
	fds.set_deleted_key(-2);

	components.set_empty_key("");

	mqueue.set_empty_key(0);
	mqueue.set_deleted_key(-1);
}

Engine::~Engine()
{
}

Engine& Engine::get_instance()
{
	static Engine instance;
	return instance;
}

TaskQueue& Engine::get_tasks()
{
	return tasks;
}

service_vec& Engine::get_services()
{
	return services;
}

Config& Engine::get_configuration()
{
	return config;
}

connection_ptr Engine::open_connection(std::string host, int port)
{
	std::string port_str = boost::lexical_cast<std::string>(port);
	int fd = SocketUtils::socket_connection_new(host.c_str(), port_str.c_str(), 0);
	if (fd < 0) return connection_ptr();
	SocketUtils::make_socket_non_blocking(fd);
	connection_ptr conn = add_connection(fd);
	return conn;
}

int Engine::listen_connection(std::string host, int port)
{
	std::string mport = boost::lexical_cast<std::string>(port);
	int listen_fd = SocketUtils::create_and_bind(mport.c_str());
	if (listen_fd == -1) return -1;

	int s = SocketUtils::make_socket_non_blocking(listen_fd);
	if (s == -1)
	{
		close(listen_fd);
		return -1;
	}

	s = listen(listen_fd, SOMAXCONN);
	if (s == -1)
	{
		DLOG(ERROR) << "Error in connection listening";
		close(listen_fd);
		return -1;
	}

	if (!watch(listen_fd))
	{
		close(listen_fd);
		return -1;
	}
	DLOG(INFO)<<"Start listing on "<<host<<":"<<port;
	fds.insert(listen_fd);
	return listen_fd;
}

void Engine::close_connection(int fd)
{
	boost::mutex::scoped_lock lock(connection_mutex);
	connections.erase(fd);
	close(fd);
}

connection_ptr Engine::add_connection(int fd)
{
	int s;
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
	if (s == -1)
	{
		DLOG(ERROR) << "Error in epoll_ctl";
		return connection_ptr();
	}
	connection_ptr conn = boost::make_shared<Connection>(fd);
	conn->me = conn;
	connection_mutex.lock();
	connections[fd] = conn;
	connection_mutex.unlock();
	return conn;
}

connection_ptr Engine::get_connection(int fd)
{
	boost::mutex::scoped_lock lock(connection_mutex);
	connection_map::iterator it = connections.find(fd);
	if (it == connections.end()) return connection_ptr();
	return it->second;
}

bool Engine::watch(int fd)
{
	int s;
	struct epoll_event event;
	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);
	if (s == -1)
	{
		DLOG(ERROR) << "Error in epoll_ctl";
		return false;
	}
	return true;
}

void Engine::main_loop()
{
	int s;
	struct epoll_event event;
	struct epoll_event *events;

	/* Buffer where events are returned */
	events = (epoll_event*) calloc(max_event, sizeof event);

	DLOG(INFO) << "Starting main loop";
	while (running)
	{
		int n, i;
		n = epoll_wait(epoll_fd, events, max_event, -1);
		for (i = 0; i < n; i++)
		{
			if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))
			{
				/* An error has occured on this fd, or the socket is not ready for reading (why were we notified then?) */
				DLOG(ERROR) << "epoll error";
				close(events[i].data.fd);
				continue;
			}
			else if (fds.count(events[i].data.fd) > 0)
			{
				/* We have a notification on the listening socket, which means one or more incoming connections. */
				while (1)
				{
					struct sockaddr in_addr;
					socklen_t in_len;
					int infd;
					char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

					in_len = sizeof in_addr;
					infd = accept(events[i].data.fd, &in_addr, &in_len);
					if (infd == -1)
					{
						if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
						{
							/* We have processed all incoming connections. */
							break;
						}
						else
						{
							perror("accept");
							break;
						}
					}

					s = getnameinfo(&in_addr, in_len, hbuf, sizeof hbuf, sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
					if (s == 0) DLOG(INFO) << "Accepted connection on descriptor " << infd << " (host=" << hbuf << ", port=" << sbuf << ")";

					/* Make the incoming socket non-blocking and add it to the list of fds to monitor. */
					s = SocketUtils::make_socket_non_blocking(infd);
					if (s == -1)
					{
						break;
					}

					connection_ptr conn = add_connection(infd);
					if (conn == NULL) close(infd);
					continue;
				}
			}
			else
			{
				/* push event to queue */
				//DLOG(INFO) << "new read event";
				connection_ptr con = get_connection(events[i].data.fd);
				if (con != NULL) con->notify_read();
				//task_ptr task = new TaskRead(events[i].data.fd);
				//tasks.enqueue(task);
			}
		}
	}

	free(events);
}

void Engine::start()
{
	running = true;

	//Hook SIGTERM (kill) and SIGINT (Ctrl-C)
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = Engine::interrupt_cb;
	if (sigaction(SIGINT, &act, 0))
	{
		DLOG(ERROR) << "Sigint action error";
	}
	if (sigaction(SIGTERM, &act, 0))
	{
		DLOG(ERROR) << "Sigterm action error";
	}

	tasks.start();
	/* start components */
	for (component_map::iterator it = components.begin(); it != components.end(); it++)
	{
		DLOG(INFO) << "Component:" << it->second->get_id();
		it->second->start();
	}

	for (worker_vec::iterator it = workers.begin(); it != workers.end(); it++)
	{
		boost::shared_ptr<Worker> w = *it;
		w->start();
	}

    DLOG(INFO) << "Server started";
    main_loop();
}

void Engine::stop()
{
	running = false;

	tasks.stop();
	/* stop components */
	for (component_map::iterator it = components.begin(); it != components.end(); it++)
	{
		DLOG(INFO) << "Component:" << it->second->get_id();
		it->second->stop();
	}

	/* close all connections */
	for (connection_map::iterator it = connections.begin(); it != connections.end(); it++)
	{
		connection_ptr conn = it->second;
		close(conn->get_socket());
	}

	/* wait for all workers finished */
	for (worker_vec::iterator it = workers.begin(); it != workers.end(); it++)
	{
		boost::shared_ptr<Worker> w = *it;
		w->join();
	}

	DLOG(INFO) << "Server stopped";
}

void Engine::interrupt_cb(int sig)
{
	Engine::get_instance().stop();
}

void Engine::init(const char *fname)
{
	DLOG(INFO) << "Loading configuration";
	try
	{
		config.readFile(fname);
	}
	catch (const FileIOException &fioex)
	{
		throw std::runtime_error("I/O error while reading file.");
	}
	catch (const ParseException &pex)
	{
		throw std::runtime_error("Parse configuration file error");
	}

	//Load server configuration here
	int number_of_worker;
	Setting &sconf = config.getRoot()["general"];
	if (!sconf.lookupValue("number_of_worker", number_of_worker))
	{
		number_of_worker = 10;
		DLOG(ERROR) << "Fail to load number_of_worker parameter from configuration file";
	}

	for (int i = 0; i < number_of_worker; i++)
	{
		boost::shared_ptr<Worker> w(new Worker(i));
		workers.push_back(w);
	}

	int task_queue_size;
	if (!sconf.lookupValue("task_queue_size", task_queue_size))
	{
		task_queue_size = 100;
		DLOG(ERROR) << "Use default EventQueue size: 100";
	}
	tasks.init(task_queue_size);

	if (!sconf.lookupValue("max_event", max_event))
	{
		max_event = 512;
		DLOG(ERROR) << "Use default max_event: 512";
	}

	/* register services */
	service_ptr core_service = boost::make_shared<CoreService>();
	register_service(core_service);
	core_service->activate();

	/* create epoll */
	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1)
	{
		throw std::runtime_error("Error in epoll_create");
	}

	/* init components */
	for (component_map::iterator it = components.begin(); it != components.end(); it++)
	{
		DLOG(INFO) << "Component:" << it->second->get_id();
		it->second->init();
	}
}

void Engine::register_service(service_ptr service)
{
	services.push_back(service);
}

void Engine::process_message(message_ptr const & message, connection_ptr const & connection)
{
	for (service_vec::iterator it = services.begin(); it != services.end(); it++)
	{
		boost::shared_ptr<Service> service = *it;
		if (service->activated() && service->process(message, connection)) break;
	}
}

/* generate a unique id for message communication */
uint64_t Engine::next_message_id()
{
	boost::mutex::scoped_lock lock(msg_id_mutex);
	msg_current_id++;
	if (msg_current_id < 2) msg_current_id = 2;
	return msg_current_id;
}

void Engine::save_context(Context* tracker)
{
	boost::mutex::scoped_lock lock(context_mutex);
	contexts[tracker->get_id()] = tracker;
}

void Engine::release_context(Context* tracker)
{
	boost::mutex::scoped_lock lock(context_mutex);
	contexts.erase(tracker->get_id());
}

Context* Engine::get_context(uint64_t id)
{
	boost::mutex::scoped_lock lock(context_mutex);
	context_map::iterator it = contexts.find(id);
	if (it == contexts.end()) return NULL;
	return it->second;
}

message_ptr Engine::wait_reply(uint64_t id, int timeout)
{
	boost::scoped_ptr<ReplyContext> context(new ReplyContext(id));
	save_context(context.get());
	context->wait(timeout);
	release_context(context.get());
	if (context->done) return context->get_reply();
	return message_ptr();
}

void Engine::register_component(component_ptr component)
{
	components[component->get_id()] = component;
}

component_ptr Engine::get_component(std::string id)
{
	component_map::iterator it = components.find(id);
	if (it != components.end()) return it->second;
	return component_ptr();
}

/* magic queue */
void Engine::mqueue_push(message_ptr const& message, Worker* worker, bool& working)
{
	boost::mutex::scoped_lock lock(mqueue_mutex);
	mqueue_map::iterator it = mqueue.find(message->okey);
	if (it != mqueue.end()) //another worker is assigned to okey
	{
		it->second->messages.push_back(message);
		working = true;
	}
	else //no worker for this okey
	{
		//worker->messages.push_back(message);
		worker->okey = message->okey;
		mqueue[message->okey] = worker;
		working = false;
	}
}

message_ptr Engine::mqueue_pop(uint32_t okey)
{
	boost::mutex::scoped_lock lock(mqueue_mutex);
	mqueue_map::iterator it = mqueue.find(okey);
	if (it != mqueue.end())
	{
		Worker* worker = it->second;
		if (worker->messages.empty())
		{
			worker->okey = 0;
			mqueue.erase(okey);
			return message_ptr();
		}
		message_ptr ret = worker->messages.front();
		it->second->messages.pop_front();
		return ret;
	}
	return message_ptr();
}

} /* namespace core */
