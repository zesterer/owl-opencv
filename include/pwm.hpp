#ifndef OWL_HPP_PWM
#define OWL_HPP_PWM

using socket_t = int;

namespace owl {
    const int EYE_L_X_CENTRE = 1545;
    const int EYE_L_Y_CENTRE = 1530;
    const int EYE_R_X_CENTRE = 1545;
    const int EYE_R_Y_CENTRE = 1500;
    const int NECK_CENTRE = 1530;

    const int EYE_L_X_MIN = 1180;
    const int EYE_L_X_MAX = 1850;
    const int EYE_L_Y_MIN = 1180;
    const int EYE_L_Y_MAX = 2000;
    const int EYE_R_X_MIN = 1180;
    const int EYE_R_X_MAX = 1890;
    const int EYE_R_Y_MIN = 1200;
    const int EYE_R_Y_MAX = 2000;
    const int NECK_MIN = 1100;
    const int NECK_MAX = 1950;

    struct Eye {
        float
            x = 0.0f,
            y = 0.0f;

        Eye() {}
        Eye(float x, float y) : x(x), y(y) {}
    };

    struct Params {
		enum class Mode {
			CONTROL,
			SINUSOIDAL,
			STEREO,
			CHAMELEON,
			CRAZY,
			CONFUSED,
			NECK_SINUSOIDAL,
			CORRELATION,
		};

		Eye eyes[2];
        float neck = 0.0f;
		Mode mode;

        Params() {}
        Params(Eye eye_l, Eye eye_r, float neck) : neck(neck), mode(Mode::CONTROL) {
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
