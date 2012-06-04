#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <stdint.h>

class SocketUtils
{
public:
	/*
	 * Bind to a port. This process does not include listening.
	 * Return the socket file descriptor
	 */
	static int create_and_bind(const char *port)
	{
		struct addrinfo hints;
		struct addrinfo *result, *rp;
		int s, sfd;
		int yes = 1;

		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_UNSPEC; /* Return IPv4 and IPv6 choices */
		hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
		hints.ai_flags = AI_PASSIVE; /* All interfaces */

		s = getaddrinfo(NULL, port, &hints, &result);
		if (s != 0)
		{
			return -1;
		}

		for (rp = result; rp != NULL; rp = rp->ai_next)
		{
			sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (sfd == -1) continue;

			if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
			{
				return -1;
			}

			s = bind(sfd, rp->ai_addr, rp->ai_addrlen);
			if (s == 0)
			{
				/* We managed to bind successfully! */
				break;
			}

			close(sfd);
		}

		if (rp == NULL)
		{
			return -1;
		}

		freeaddrinfo(result);
		return sfd;
	}

	/*
	 * Make a socket non blocking.
	 * Return -1 on error.
	 */
	static int make_socket_non_blocking(int fd)
	{
		int flags, s;

		flags = fcntl(fd, F_GETFL, 0);
		if (flags == -1)
		{
			return -1;
		}

		flags |= O_NONBLOCK;
		s = fcntl(fd, F_SETFL, flags);
		if (s == -1)
		{
			return -1;
		}

		return 0;
	}

	/*
	 * Use TCP_NODELAY
	 */
	static int disable_nagle(int fd)
	{
		int flag = 1;
		if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)) < 0)
		{
			return -1;
		}
		return 0;
	}

	/*
	 * Connect to a server.
	 * Return the socket file descriptor
	 */
	static int socket_connection_new(const char *host, const char *port, int timeout)
	{
		int status, socketfd;
		struct addrinfo hints;
		struct addrinfo *servinfo;

		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		//Get host info
		if ((status = getaddrinfo(host, port, &hints, &servinfo)) != 0)
		{
			return -1;
		}

		//Opening socket
		socketfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
		if (socketfd == -1)
		{
			freeaddrinfo(servinfo);
			return -1;
		}

		int yes = 1;
		if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			freeaddrinfo(servinfo);
			return -1;
		}

		//Set timeout
		if (timeout > 0)
		{
			struct timeval t;
			t.tv_sec = timeout;
			t.tv_usec = 0;
			if (setsockopt(socketfd, SOL_SOCKET, SO_RCVTIMEO, (void *) &t, sizeof(t)) < 0)
			{
				freeaddrinfo(servinfo);
				return -1;
			}

			if (setsockopt(socketfd, SOL_SOCKET, SO_SNDTIMEO, (void *) &t, sizeof(t)) < 0)
			{
				freeaddrinfo(servinfo);
				return -1;
			}
		}

		//Connect
		if (connect(socketfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
		{
			return -1;
		}

		freeaddrinfo(servinfo);
		return socketfd;
	}
};

#endif
