#ifndef HTTP_H_
#define HTTP_H_

#include "connection.h"

#include <map>

enum {
	HTTP_OK = 0,
	HTTP_INVALID_REQUEST,
	HTTP_UNSUPPORTED_HTTP_VERSION
};

enum class HttpMethod : int {
	GET = 0,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE,
	PATCH,

	UNKNOWN_METHOD /* Invalid method. */
};

class HttpServer {
public:
	using std::map<std::string, std::string> = Table;

	InetListener listener;
	Table resource_map;
	int port;

	void set_port(int port);
	void start();
	void mainloop();
	void add_resource_handler(std::string resource, std::string path, );

	void request_handler(InetConnection client);
};

class HttpRequest {
public:
    HttpMethod method;
    std::string resource;
    int major;
    int minor;
    std::map<std::string, std::string> headers;
    // std::string body;

    int parse_request_line(const std::string& s);
    int add_header(const std::string& s);
    // int set_body(const std::string& s);
};

class HttpResponse {
public:
	int major;
	int minor;
	int status;
	std::string status_str;
	std::map<std::string, std::string> headers;	
	// std::string body;

	std::string get_as_string();
	void set_version(int major, int minor);
	void set_status(int status, std::string& status_str);
	void set_status(int status, std::string&& status_str);
	void add_header(const std::string& key, const std::string& value);
	void add_header(const std::string&& key, const std::string&& value);
};

#endif
