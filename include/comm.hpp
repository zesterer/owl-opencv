#ifndef OWL_HPP_COMM
#define OWL_HPP_COMM

// Std
#include <mutex>
#include <atomic>
#include <string>
#include <optional>

using socket_t = int;

namespace owl {
	struct Params {
		struct { float x = 0.0f, y = 0.0f; } eye[2];
		float neck = 0.0f;
	};

	struct Connection {
		private:
			std::unique_lock<std::mutex> lock;
			socket_t sock;

			Connection(socket_t sock) : sock(sock) {}
		public:
			std::atomic<Params> params;

			Connection (Connection&& other) {
				this->lock = std::move(other.lock);
				this->sock = other.sock;
			}

			static std::optional<Connection> from(std::string addr, int port);
	};
}

#endif
