#include "http.h"

#include <sstream>

std::string HttpResponse::get_as_string() {
	std::ostringstream response;

	response << "HTTP/" << this->major << '.' << this->minor
			 << ' ' << this->status << ' ' << this->status_str << "\r\n";

	for (auto& pair : this->headers)
		response << pair.first << ": " << pair.second << "\r\n";

	response << "\r\n";

	return response.str();
}

void HttpResponse::set_version(int major, int minor) {
	this->major = major;
	this->minor = minor;
}

void HttpResponse::set_status(int status, std::string&& status_str) {
	this->set_status(status, status_str);
}

void HttpResponse::set_status(int status, std::string& status_str) {
	this->status = status;
	this->status_str = status_str;
}

void HttpResponse::add_header(const std::string& key, const std::string& value) {
	headers[key] = value;
}

void HttpResponse::add_header(const std::string&& key, const std::string&& value) {
	this->add_header(key, value); // This is not recursive.
}
	