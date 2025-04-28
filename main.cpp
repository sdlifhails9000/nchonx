#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

#include "http.h"
#include "connection.h"

static std::map<std::string, std::string> resource_map;

void *handle_request(void *arg) {
	InetConnection client = *static_cast<InetConnection*>(arg);
    HttpRequest req;
    std::ifstream f;
    std::ostringstream response;
	std::string request_line;

	if (client.recv(request_line, 1024) != 0) {
		std::cerr << "whoops" << std::endl;
		return nullptr;
	}

    req.parse_request_line(request_line);

	if (req.method != HttpMethod::GET) {
		response << "HTTP/1.1 405 Method Not Allowed\r\n";
		response << "Content-Length: 0\r\n";
		response << "Allowed: GET\r\n\r\n";

		client.send(response.str());
		client.close();
		return nullptr;
	}

	std::string file;
	std::map<std::string, std::string>::iterator it = resource_map.find(req.resource);

	if (it == resource_map.end()) {
		file = "httpdoc/notfound.html";
		response << "HTTP/1.1 404 Not found\r\n";
	} else {
		file = it->second;
		response << "HTTP/1.1 200 Good\r\n";
	}	

	f.open(file, std::ios::binary);
	f.seekg(0, f.end);
	int n_left = f.tellg();
	f.seekg(0, f.beg);

	response << "Transfer-Encoding: chunked\r\n";

	if (file == "httpdoc/download.jpg")
		response << "Content-Type: image/jpeg\r\n";

	response << "\r\n";

	client.send(response.str());
	response.str("");
	response.clear();

	char f_buff[101] = {};
	int amount = 100;
	while (n_left > 0) {
		f.read(f_buff, amount);
		n_left -= f.gcount();

		/* Creating the data chunk. */
		response << std::hex << amount << "\r\n"; /* Chunk size */
		response.write(f_buff, amount); /* This works for binary files as well. */
		response << "\r\n"; /* Chunk end. */

		if (n_left < 100)
			amount = n_left;

		client.send(response.str());
	
		memset(f_buff, '\0', sizeof(f_buff));
		response.str("");
		response.clear();
	}

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
