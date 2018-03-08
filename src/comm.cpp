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
    #include <unistd.h>
#endif

// Std
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <errno.h>

const float PI = 3.141592;

namespace owl {
    std::mutex test_lock;

	void Connection::out_func(Connection& conn) {
		int i = 0;

		// Confused state things
		int confused_hold = 0;
		enum class ConfusedState {BTM_LEFT, RIGHT, ROLL, LEFT};
		ConfusedState confused_state = ConfusedState::BTM_LEFT;

		for (float t = 0;; t += 0.1f) {
			// Get target parameters
			Params params = conn.get_params();

			if (params.mode == Params::Mode::SINUSOIDAL) {
				params = Params::centre();
				params.eyes[0].x += std::sin(t) * 200.0f;
				params.eyes[0].y += std::cos(t) * 200.0f;
				params.eyes[1].x += std::sin(t) * 200.0f;
				params.eyes[1].y -= std::cos(t) * 200.0f;
			}
			else if (params.mode == Params::Mode::STEREO) {
				params = Params::centre();
				params.eyes[0].x += std::sin(t * 0.5) * 200.0f;
				params.eyes[1].x += std::sin(t * 0.5) * 200.0f;
			}
			else if (params.mode == Params::Mode::CHAMELEON) {
				if (i % 10 == 0) {
					if (std::rand() % 5 == 0) {
						params.eyes[0].x = EYE_L_X_CENTRE + std::sin(std::rand() % 1000) * 200.0f;
						params.eyes[0].y = EYE_L_Y_CENTRE + std::sin(std::rand() % 1000) * 200.0f;
					}
					if (std::rand() % 5 == 0) {
						params.eyes[1].x = EYE_R_X_CENTRE + std::sin(std::rand() % 1000) * 200.0f;
						params.eyes[1].y = EYE_R_Y_CENTRE + std::sin(std::rand() % 1000) * 200.0f;
					}
					if (std::rand() % 10 == 0) {
						params.neck = NECK_CENTRE + std::sin(std::rand() % 1000) * 300.0f;
					}
				}

				conn.set_params(params);
			}
			else if (params.mode == Params::Mode::CRAZY) {
				params = Params::centre();
				params.eyes[0].x += std::sin(PI * 2.0f * std::sin(t * 0.2f) * 0.3f) * 200.0f;
				params.eyes[0].y += std::cos(PI * 2.0f * std::sin(t * 0.25f) * 0.5f) * 200.0f;
				params.eyes[1].x += std::sin(PI * 2.0f * std::sin(t * 0.3f) * 0.6f) * 200.0f;
				params.eyes[1].y -= std::cos(PI * 2.0f * std::sin(t * 0.15f) * 0.4f) * 200.0f;
			}
			else if (params.mode == Params::Mode::CONFUSED) { //mode to copy motion of gif: https://media3.giphy.com/media/ZQrVQtav6gnzG/giphy.gif
				params = Params::centre();
				switch (confused_state) {
				case ConfusedState::BTM_LEFT: { // Start bottom-left
						params.eyes[0].x = EYE_L_X_MIN;
						params.eyes[0].y = EYE_L_Y_MAX;
						params.eyes[1].x = EYE_R_X_MIN;
						params.eyes[1].y = EYE_R_Y_MIN;
						if (confused_hold < 50) {
							confused_hold ++;
						} else {
							confused_hold = 0;
							confused_state = ConfusedState::RIGHT;
						}
						break;
					}
				case ConfusedState::RIGHT: { // Look to the right
						params.eyes[0].x = EYE_L_X_MAX;
						params.eyes[1].x = EYE_R_X_MAX;
						if (confused_hold < 50) {
							confused_hold ++;
						} else {
							confused_hold = 0;
							confused_state = ConfusedState::ROLL;
							t = 0;
						}
						break;
					}
				case ConfusedState::ROLL: { // Roll the eyes
						float nt = t + PI / 2.f;
						params.eyes[0].x += std::sin(nt) * 300.0f;
						params.eyes[0].y += std::cos(nt) * 300.0f;
						params.eyes[1].x += std::sin(nt) * 300.0f;
						params.eyes[1].y -= std::cos(nt) * 300.0f;
						if (std::fmod(t, 2 * PI) > PI) { // Stop roll at top-left (x-centre with +150 offset)
							confused_hold = 0;
							confused_state = ConfusedState::LEFT;
						}
						break;
					}
				case ConfusedState::LEFT: { // Look to the left
						params.eyes[0].x = EYE_L_X_MIN;
						params.eyes[1].x = EYE_R_X_MIN;
						if (confused_hold < 100) {
							confused_hold ++;
						} else {
							confused_hold = 0;
							confused_state = ConfusedState::BTM_LEFT;
						}
						break;
					}
				default: { //Default to centre
						break;
					}
				}
			}
			else if (params.mode == Params::Mode::NECK_SINUSOIDAL) {
				params = Params::centre();
				params.neck += std::sin(t * 0.5f) * 300.0f;
			}

			// Construct PWM packet
			std::stringstream packet;
			packet <<
			    (int)params.eyes[1].x << " " <<
			    (int)params.eyes[1].y << " " <<
			    (int)params.eyes[0].x << " " <<
			    (int)params.eyes[0].y << " " <<
			    (int)params.neck;

            #ifdef __WIN32__
			    int smsg=send(conn.sock,packet.str().c_str(),packet.str().length(),0);
				if(smsg==SOCKET_ERROR){
					std::cout<<"Error: "<<WSAGetLastError()<<std::endl;
					WSACleanup();
				}
				char recv_buff[2];
				int N=recv(conn.sock,recv_buff,2,0);
				if(N==SOCKET_ERROR){
					std::cout<<"Error in Receiving: "<<WSAGetLastError()<<std::endl;
				}
            #else
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
            #endif

			// Put the thread to sleep for a while
			std::this_thread::sleep_for(std::chrono::milliseconds(10));

			i ++;
		}
	}

    #if __cplusplus >= 201703L
	    std::optional<Connection> Connection::from(std::string ip, int port) {
    #else
	    Connection Connection::from(std::string ip, int port) {
    #endif
        #ifdef __WIN32__
			WSAData version;		//We need to check the version.
		WORD mkword=MAKEWORD(2,2);
		int what=WSAStartup(mkword,&version);
		if(what!=0){
			std::cout<<"This WSA networking version is not supported! - \n"<<WSAGetLastError()<<std::endl;
		}
		else{
			std::cout<<"WSA networking initiated\n"<<std::endl;
		}

		SOCKET u_sock=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(u_sock==INVALID_SOCKET)
			std::cout<<"Creating socket fail\n";

		else
			std::cout<<"It was okay to create the socket\n";
		    //Socket address information
		    sockaddr_in addr;
			addr.sin_family=AF_INET;
			addr.sin_addr.s_addr=inet_addr(ip.c_str());
			addr.sin_port=htons(port); // HAS to match that defined in Pi server
			/*==========Addressing finished==========*/

			//Now we connect
			int conn=connect(u_sock,(SOCKADDR*)&addr,sizeof(addr));
			if(conn==SOCKET_ERROR){
				std::cout<<"Error - when connecting "<<WSAGetLastError()<<std::endl;
				closesocket(u_sock);
				WSACleanup();
			}

			return Connection(u_sock);
        #else
			// Attempt to create a new POSIX socket
			socket_t tmp_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (tmp_sock < 0) {
				std::cerr << "Failed to create POSIX socket: " << errno << std::endl;
                #if __cplusplus >= 201703L
				    return {};
                #else
				    exit(1);
                #endif
			}
			else { // We have a valid socket
				// Define the address we want to connect to
				sockaddr_in addr;
				std::memset(&addr, 0, sizeof addr);
				addr.sin_family = AF_INET;
				addr.sin_port = port;
				//addr.sin_addr.s_addr = inet_addr(ip.c_str());
				inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

				if (connect(
				    tmp_sock,
				    (struct sockaddr*)&addr,
				    sizeof addr
				) == -1) {
					std::cerr << "Failed to connect to '" << ip.c_str() << ":" << port << "': " << errno << std::endl;

					// Close the socket on failure
					close(tmp_sock);

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
        #endif
	}
}
