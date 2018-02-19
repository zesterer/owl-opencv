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
		std::cout << "  --video-url <url>  Specify the video stream URL" << std::endl;
		std::cout << "  --ip        <addr> Specify the command server IP address" << std::endl;
		std::cout << "  --port      <port> Specify the command server port" << std::endl;
	}

	extern "C" int main(int argc, char* argv[]) {
		std::string video_url = "http://10.0.0.10:8080/stream/video.mjpeg";
		std::string ip = "10.0.0.10";
		int port = 12345;

		enum class ArgState { DEFAULT, VIDEO_URL, IP, PORT };
		ArgState state = ArgState::DEFAULT;
		for (int i = 1; i < argc; i ++) {
			auto arg = std::string(argv[i]);

			switch (state) {
				case ArgState::DEFAULT: {
					if (arg == "--video-url") {
						state = ArgState::VIDEO_URL;
					}
					else if (arg == "--ip") {
						state = ArgState::IP;
					}
					else if (arg == "--port") {
						state = ArgState::PORT;
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

				case ArgState::IP: {
					ip = arg;
					state = ArgState::DEFAULT;
					break;
				}

				case ArgState::PORT: {
					if (std::sscanf(arg.c_str(), "%d", &port) != 1) {
						std::cerr << "Expected integer after '--port', not '" << arg << "'." << std::endl;
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

			case ArgState::IP: {
				std::cerr << "Expected argument after '--ip'" << std::endl;
				return 1;
			}

			case ArgState::PORT: {
				std::cerr << "Expected argument after '--port'" << std::endl;
				return 1;
			}

			default: break;
		}

		return run(video_url, ip, port);
	}
}
