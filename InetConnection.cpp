#include "connection.h"

#include <iostream>

int InetConnection::connect(const char *addr, const char *port) {
	addrinfo *res = nullptr;
	addrinfo hints{};

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(addr, port, &hints, &res);	
	for (addrinfo *iter = res; iter != nullptr; iter = iter->ai_next) {
		if ((this->sock = socket(iter->ai_family, iter->ai_socktype, iter->ai_protocol)) == INVALID_SOCKET)
			continue;

		if (::connect(this->sock, iter->ai_addr, iter->ai_addrlen) == 0)
			break;

		this->sock = INVALID_SOCKET;
		if (closesocket(this->sock) != 0)
			return ERROR;

		this->sock = INVALID_SOCKET;
		continue;
	}
	freeaddrinfo(res);

	if (this->sock == INVALID_SOCKET) {
		std::cerr << "ERROR: Can't connect to any address." << std::endl;
		return ERROR;
	}

	return OK;
}

int InetConnection::recv(std::string& s, int amount) {
	char *buffer = new char[amount] {};

	if (::recv(this->sock, buffer, amount, 0) <= 0) {
		delete[] buffer;
		return -1;
	}

	s = buffer;
	delete[] buffer;

	return 0;
}

int InetConnection::send(const std::string&& s) {
	return this->send(s);
}

int InetConnection::send(const std::string& s) {
	if (::send(this->sock, s.c_str(), s.length(), 0) != 0)
		return ERROR;

	return OK;
}


int InetConnection::close() {
	if (closesocket(this->sock) != 0)
		return ERROR;

	return OK;
}
