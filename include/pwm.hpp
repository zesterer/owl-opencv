#ifndef OWL_HPP_PWM
#define OWL_HPP_PWM

// Std
#include <mutex>
#include <thread>
#include <atomic>
#include <string>
#include <cstdlib>
#include <optional>
#include <iostream>

using socket_t = int;

namespace owl {
	const int EYE_L_X_CENTRE = 1545;
	const int EYE_L_Y_CENTRE = 1460;
	const int EYE_R_X_CENTRE = 1545;
	const int EYE_R_Y_CENTRE = 1460;
	const int NECK_CENTRE = 1530;

	struct Eye {
		float
			x = 0.0f,
			y = 0.0f;

		Eye() {}
		Eye(float x, float y) : x(x), y(y) {}
	};

	struct Params {
		Eye eyes[2];
		float neck = 0.0f;

		Params() {}
		Params(Eye eye_l, Eye eye_r, float neck) : neck(neck) {
			this->eyes[0] = eye_l;
			this->eyes[1] = eye_r;
		}

		static Params centre() {
			return Params(
				Eye(EYE_L_X_CENTRE, EYE_L_Y_CENTRE),
				Eye(EYE_R_X_CENTRE, EYE_R_Y_CENTRE),
				NECK_CENTRE
			);
		}
	};

	const Params CENTRE_PARAMS = Params::centre();
}

#endif
