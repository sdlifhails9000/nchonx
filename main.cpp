#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <map>

#include "http.h"
#include "connection.h"

static std::map<std::string, std::string> resource_map;

void recvline(InetConnection& c, std::string& s) {
	std::string ch:
	do {
		c.recv(ch, 1);
		s += ch;
		ch.clear();
	} while (ch != "\n");
}

void *handle_request(void *arg) {
	InetConnection client = *static_cast<InetConnection*>(arg);
    std::ifstream f;
	std::string request_line;
    HttpRequest req;
    HttpResponse resp;
    
    resp.set_version(1, 1);

	if (client.recv(request_line, 1024) != 0) {
		std::cerr << "whoops" << std::endl;
		return nullptr;
	}

    if (req.parse_request_line(request_line) == HTTP_INVALID_REQUEST) {
    	resp.set_status(400, "Malformed request");
		resp.add_header("Content-Length", "0");

		client.send(resp.get_as_string());
		client.close();
		return nullptr;
    }

	if (req.method != HttpMethod::GET) {
		resp.set_status(501, "Not implemented");
		resp.add_header("Content-Length", "0");
		resp.add_header("Allowed", "GET");

		client.send(resp.get_as_string());
		client.close();
		return nullptr;
	}

	std::string file;
	std::map<std::string, std::string>::iterator it = resource_map.find(req.resource);

	if (it == resource_map.end()) {
		file = "httpdoc/notfound.html";
		resp.set_status(404, "Not found");
	} else {
		file = it->second;
		resp.set_status(200, "OK");
	}	

	resp.add_header("Transfer-Encoding", "chunked");

	f.open(file, std::ios::binary);
	f.seekg(0, f.end);
	int n_left = f.tellg();
	f.seekg(0, f.beg);

	client.send(resp.get_as_string());

	std::ostringstream oss;
	char f_buff[101] = {};
	int amount = 100;

	while (n_left > 0) {
		f.read(f_buff, amount);
		n_left -= f.gcount();

		/* Creating the data chunk. */
		oss << std::hex << amount << "\r\n"; /* Chunk size */
		oss.write(f_buff, amount); /* This works for binary files as well. */
		oss << "\r\n"; /* Chunk end. */
		
		client.send(oss.str());

		if (n_left < 100)
			amount = n_left;

		std::fill(f_buff, f_buff + sizeof(f_buff), '\0');
		oss.str("");
		oss.clear();
	}

    /* Final data chunk. */
	client.send("0\r\n\r\n");

	f.close();
	client.close();
	return nullptr;
}

int main(void) {
	InetListener l;
	l.bind("1234", false);
	l.listen(10);

	resource_map["/"] = "httpdoc/index.html";
	resource_map["/about"] = "httpdoc/about.html";
	resource_map["/login"] = "httpdoc/login.html";

	while (true) {
		InetConnection client = {};
		l.accept(client);
		handle_request(&client);
	}

	l.shutdown(0);
	l.close();

	return 0;
}

