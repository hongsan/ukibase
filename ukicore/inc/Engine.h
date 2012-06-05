/*
 * Manager.h
 *
 *  Created on: Jan 19, 2012
 *      Author: nhsan
 */

#ifndef ENGINE_H_
#define ENGINE_H_
#include <boost/smart_ptr/shared_ptr.hpp>
#include <google/dense_hash_map>
#include <google/dense_hash_set>
#include <boost/thread/mutex.hpp>
#include <vector>
#include "Component.h"
#include "Connection.h"
#include "Service.h"
#include "Worker.h"
#include "TaskQueue.h"
#include "ServerNode.h"
#include <libconfig.h++>
#include "Context.h"

using namespace libconfig;

namespace ukicore
{
typedef boost::shared_ptr<Component> component_ptr;
typedef google::dense_hash_map<int, connection_ptr> connection_map;
typedef google::dense_hash_map<int, servernode_ptr> servernode_map;
typedef std::vector<service_ptr> service_vec;
typedef std::vector<worker_ptr> worker_vec;
typedef google::dense_hash_map<uint64_t, Context*> context_map;
typedef google::dense_hash_map<std::string, component_ptr> component_map;
typedef google::dense_hash_map<uint32_t, Worker*> mqueue_map;
typedef google::dense_hash_set<int> listenfd_set;

class Engine
{
private:

	/* Singleton */
	Engine();
	Engine(Engine const&);
	void operator=(Engine const&);
	virtual ~Engine();

	Config config;
	component_map components;
	connection_map connections;
	boost::mutex connection_mutex;
	service_vec services;
	worker_vec workers;
	TaskQueue tasks;

	/* epoll */
	int epoll_fd;
	int max_event;
	listenfd_set fds;

	/* context */
	context_map contexts;
	boost::mutex context_mutex;

	/* unique message id */
	boost::mutex msg_id_mutex;
	uint64_t msg_current_id;

	/* magic queue */
	mqueue_map mqueue;
	boost::mutex mqueue_mutex;
public:
	bool running;
	static Engine& get_instance();
	void main_loop();
	void start();
	void stop();
	void init(const char *fname);

	/* connections */
	bool watch(int fd);
	connection_ptr open_connection(std::string host, int port);
	connection_ptr add_connection(int fd);
	void close_connection(int fd);
	connection_ptr get_connection(int fd);
	int listen_connection(std::string host, int port);

	/* getters */
	TaskQueue& get_tasks();
	service_vec& get_services();
	Config& get_configuration();


	/* service routines */
	void register_service(service_ptr service);
	void process_message(message_ptr const & message, connection_ptr const & connection);

	/* context */
	void save_context(Context* tracker);
	Context* get_context(uint64_t id);
	void release_context(Context* tracker);
	message_ptr wait_reply(uint64_t id, int timeout = 10);

	/* unique message id */
	uint64_t next_message_id();

	/* signal handler */
	static void interrupt_cb(int sig);

	/* component */
	void register_component(component_ptr component);
	component_ptr get_component(std::string id);

	/* magic queue */
	void mqueue_push(message_ptr const& message, Worker* worker, bool& working);
	message_ptr mqueue_pop(uint32_t okey);
};

} /* namespace core */
#endif /* MANAGER_H_ */
