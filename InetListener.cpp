#include "connection.h"

#include <iostream>

__attribute__((constructor))
static void __init() {
	WSADATA wsadata = {};

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		std::cerr << "ERROR: WSAStartup" << std::endl;
		exit(1);
	}

	if (LOBYTE(wsadata.wVersion) != 2 && HIBYTE(wsadata.wVersion) != 2) {
		std::cerr << "ERROR: Couldn't find winsock version 2.2." << std::endl;
		exit(1);
	}
}

__attribute__((destructor))
static void __fini() {
	WSACleanup();
}

int InetListener::bind(const char *port, bool is_loopback) {
	addrinfo hints = {};
	addrinfo *res = nullptr;
	std::string addr;
	char yes = 0x01;

	if (is_loopback)
		addr = "127.0.0.1";

	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;

	getaddrinfo(addr.c_str(), port, &hints, &res);

	for (struct addrinfo *iter = res; iter != nullptr; iter = iter->ai_next) {
		if ((this->sock = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol)) == INVALID_SOCKET)
			continue;

		setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

		if (::bind(this->sock, iter->ai_addr, iter->ai_addrlen) != 0) {
			closesocket(this->sock);
			this->sock = INVALID_SOCKET;
			continue;
		}

		break;
	}
	freeaddrinfo(res);

	if (this->sock == INVALID_SOCKET) {
		std::cerr << "ERROR: Can't bind to any address." << std::endl;
		return INET_ERROR;
	}

	return INET_OK;
}

int InetListener::listen(int backlog) {
	if (::listen(this->sock, backlog) != 0)
		return INET_ERROR;

	return INET_OK;
}

int InetListener::accept(InetConnection& c) {
	if ((c.sock = ::accept(this->sock, NULL, NULL)) == INVALID_SOCKET)
		return INET_ERROR;

	return INET_OK;
}

int InetListener::shutdown(int type) {
	if (::shutdown(this->sock, type) != 0)
		return INET_ERROR;

	return INET_OK;
}

int InetListener::close() {
	if (closesocket(this->sock) != 0)
		return INET_ERROR;

	return INET_OK;
}
