#ifndef HTTP_H_
#define HTTP_H_

#include <map>

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

enum HttpVersion : int {
    HTTP_0_9 = 0,
    HTTP_1_0,
    HTTP_1_1,
    HTTP_2_0,
    HTTP_3_0,
    
    UNKNOWN_VERSION
};

class HttpRequest {
public:
    HttpMethod method;
    std::string resource;
    HttpVersion version;
    std::map<std::string, std::string> headers;
    std::string body;

    int parse_request_line(const std::string& s);
    int add_header(const std::string& s);
    int set_body(const std::string& s);
};

class HttpResponse {
public:
	HttpVersion version;
	// HttpStatus status;
	std::map<std::string, std::string> headers;
	std::string body;

	std::string get_as_string();
	void set_version(const HttpVersion version);
	// void set_status(const HttpStatus status);
	void add_header(const std::string& s);
};

#endif
