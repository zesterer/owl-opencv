// Local
#include <comm.hpp>

// Lib

#ifdef __WIN32__
	#include <winsock2.h>
	#include <windows.h>
#else
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
#endif

// Std
#include <iostream>
#include <cstdlib>

namespace owl {
	void Connection::out_func(Connection& conn) {
		while (true) {
			// Get target parameters
			conn.lock.lock();
			Params p = conn.tgt_params;
			conn.lock.unlock();

			// Construct PWM packet
			std::stringstream packet;
			packet <<
				p.eyes[1].x << " " <<
				p.eyes[1].y << " " <<
				p.eyes[0].x << " " <<
				p.eyes[0].y << " " <<
				p.neck;

			// Attempt to send the packet
			int send_status = send(conn.sock, packet.str().c_str(), packet.str().length(), MSG_NOSIGNAL);
			if (send_status == -1) {
				std::cerr << "Failed to send connection packet" << std::endl;
			}

			// Receive a response
			char recv_buff[2];
			int recv_status = recv(conn.sock, recv_buff, sizeof(recv_buff) / sizeof(char), 0);
			if (recv_status == -1) {
				std::cerr << "Failed to receive response packet" << std::endl;
			}

			// Put the thread to sleep for a while
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}

	#if __cplusplus >= 201703L
		std::optional<Connection> Connection::from(std::string ip, int port) {
	#else
		Connection Connection::from(std::string ip, int port) {
	#endif
		// Attempt to create a new POSIX socket
		socket_t tmp_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (tmp_sock < 0) {
			std::cerr << "Failed to create POSIX socket." << std::endl;
			#if __cplusplus >= 201703L
				return {};
			#else
				exit(1);
			#endif
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
				#if __cplusplus >= 201703L
					return {};
				#else
					exit(1);
				#endif
			}
			else {
				return Connection(tmp_sock);
			}
		}
	}
}
