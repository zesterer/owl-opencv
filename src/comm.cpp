// Local
#include <comm.hpp>

// POSIX
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

// Std
#include <iostream>

namespace owl {
	std::optional<Connection> Connection::from(std::string ip, int port) {
		// Attempt to create a new POSIX socket
		socket_t tmp_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (tmp_sock < 0) {
			std::cerr << "Failed to create POSIX socket." << std::endl;
			return {};
		}
		else { // We have a valid socket
			// Define the address we want to connect to
			sockaddr_in addr;
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = inet_addr(ip.c_str());
			addr.sin_port = port;

			if (connect(
				tmp_sock,
				(struct sockaddr*)&addr,
				sizeof(struct sockaddr)
			) != 0) {
				std::cerr << "Failed to connect to '" << ip << ":" << port << "'." << std::endl;
				return {};
			}
			else {
				return Connection(tmp_sock);
			}
		}
	}
}
