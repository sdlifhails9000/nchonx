#ifndef CONNECTION_H_
#define CONNECTION_H_

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x600
#endif

#include <string>

#include <winsock2.h>
#include <ws2tcpip.h>

enum {
	INET_ERROR = 0,
	INET_OK
};

class InetConnection {
public:
	SOCKET sock;

	InetConnection() : sock(INVALID_SOCKET)
	{}

	InetConnection(InetConnection& c) {
		this->sock = c.sock;
	}

	int connect(const char *addr, const char *port);
	int recv(std::string& s, int amount);
	int send(const std::string& s);
	int send(const std::string&& s);
	int close();
};

class InetListener {
public:
	SOCKET sock;
	int backlog;

	InetListener() : sock(INVALID_SOCKET), backlog(0)
	{}

	int bind(const char *port, bool is_loopback);
	int listen(int backlog);
	int accept(InetConnection& c);
	int shutdown(int type);
	int close();	
};

#endif
