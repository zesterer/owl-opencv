// Local
#include <run.hpp>

// Std
#include <iostream>
#include <cstdio>

namespace owl {
	void display_help() {
		std::cout << "Usage: owl [OPTIONS]" << std::endl;
		std::cout << "OPTIONS:" << std::endl;
		std::cout << "  --help             Display this help message" << std::endl;
		std::cout << "  --video-src <url>  Specify the video stream URL" << std::endl;
		std::cout << "  --pi-addr   <addr> Specify the Raspberry Pi's IP address" << std::endl;
		std::cout << "  --pi-port   <port> Specify the Raspberry Pi's command port" << std::endl;
	}

	extern "C" int main(int argc, char* argv[]) {
		std::string video_url = "http://10.0.0.10:8080/stream/video.mjpeg";
		std::string pi_addr = "10.0.0.10";
		int pi_port = 12345;

		enum class ArgState { DEFAULT, VIDEO_URL, PI_ADDR, PI_PORT };
		ArgState state = ArgState::DEFAULT;
		for (int i = 1; i < argc; i ++) {
			auto arg = std::string(argv[i]);

			switch (state) {
				case ArgState::DEFAULT: {
					if (arg == "--video-src") {
						state = ArgState::VIDEO_URL;
					}
					else if (arg == "--pi-addr") {
						state = ArgState::PI_ADDR;
					}
					else if (arg == "--pi-port") {
						state = ArgState::PI_PORT;
					}
					else if (arg == "--help") {
						display_help();
						return 0;
					}
					else {
						std::cerr << "Invalid argument '" << arg << "'." << std::endl;
						return 1;
					}
					break;
				}

				case ArgState::VIDEO_URL: {
					video_url = arg;
					state = ArgState::DEFAULT;
					break;
				}

				case ArgState::PI_ADDR: {
					pi_addr = arg;
					state = ArgState::DEFAULT;
					break;
				}

				case ArgState::PI_PORT: {
					if (std::sscanf(arg.c_str(), "%d", &pi_port) != 1) {
						std::cerr << "Expected integer after '--pi-port', not '" << arg << "'." << std::endl;
						return 1;
					}
					state = ArgState::DEFAULT;
					break;
				}

				default: break;
			}
		}

		switch (state) {
			case ArgState::VIDEO_URL: {
				std::cerr << "Expected argument after '--video-url'" << std::endl;
				return 1;
			}

			case ArgState::PI_ADDR: {
				std::cerr << "Expected argument after '--pi-addr'" << std::endl;
				return 1;
			}

			case ArgState::PI_PORT: {
				std::cerr << "Expected argument after '--pi-port'" << std::endl;
				return 1;
			}

			default: break;
		}

		return run(video_url, pi_addr, pi_port);
	}
}
