#ifndef OWL_HPP_COMM
#define OWL_HPP_COMM

// Local
#include <pwm.hpp>

// Std
#include <mutex>
#include <thread>
#include <atomic>
#include <string>
#include <sstream>
#include <cstdlib>
#include <optional>
#include <iostream>

using socket_t = int;

namespace owl {
	struct Connection {
		private:
			bool started = false;
			Params tgt_params;
			Params interp_params;
			std::thread out_thread;
			std::unique_lock<std::mutex> lock;
			socket_t sock;

			static void out_func(Connection& conn);

			Connection(socket_t sock) : sock(sock) {}
		public:
			Params get_params() {
				this->lock.lock();
				Params p = this->tgt_params;
				this->lock.unlock();
				return p;
			}

			void set_params(const Params& p) {
				this->lock.lock();
				this->tgt_params = p;
				this->lock.unlock();
			}

			void start() {
				this->started = true;
				this->out_thread = std::thread(Connection::out_func, std::ref(*this));

				this->set_params(CENTRE_PARAMS);
			}

			Connection (Connection&& other) {
				if (this->started) {
					std::cerr << "Tried to move Connection instance after thread has started" << std::endl;
					std::exit(1);
				}

				this->tgt_params = other.tgt_params;
				this->lock = std::move(other.lock);
				this->sock = other.sock;
			}

			static std::optional<Connection> from(std::string addr, int port);
	};
}

#endif
