#include <string.h>

#include "http.h"

static HttpMethod str_to_method(const std::string& method) {
    if (method == "GET")
        return HttpMethod::GET;
    else if (method == "POST")
        return HttpMethod::POST;
    else if (method == "PATCH")
        return HttpMethod::PATCH;
    else if (method == "PATCH")
        return HttpMethod::PATCH;
    else if (method == "PATCH")
        return HttpMethod::PATCH;
    else if (method == "PATCH")
        return HttpMethod::PATCH;
    else
        return HttpMethod::UNKNOWN_METHOD;
}

static HttpVersion str_to_version(const std::string& version) {
	if (version == "HTTP/0.9")
		return HttpVersion::HTTP_0_9;
	else if (version == "HTTP/1.0")
		return HttpVersion::HTTP_1_0;
	else if (version == "HTTP/1.1")
		return HttpVersion::HTTP_1_1;
	else if (version == "HTTP/2")
		return HttpVersion::HTTP_2_0;
	else if (version == "HTTP/3")
		return HttpVersion::HTTP_3_0;

	return HttpVersion::UNKNOWN_VERSION;
}

int HttpRequest::parse_request_line(const std::string& line) {
	enum {
		s_method = 0,
		s_resource,
		s_version,
		s_end
	} state = s_method;

	std::string method;
	std::string resource;
    std::string version;
	
	for (const char& c : line) {
		switch (state) {
		case s_method:
			if (c == ' ') {
				state = s_resource;
				this->method = str_to_method(method);
				break;
			}

            method.push_back(c);
			break;

		case s_resource:
			if (c == ' ') {
				state = s_version;
				this->resource = resource;
				break;
			}

            resource.push_back(c);
            break;

		case s_version:
		 	if (c == '\r') {
                state = s_end;
                this->version = str_to_version(version);
                break;
		 	}

            version.push_back(c);
		 	break;

		case s_end:
			goto end;
		}
	}

end:
	return 0;
}
