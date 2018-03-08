// Local
#include <run.hpp>
#include "comm.hpp"

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Std
#include <iostream>
#include <thread>
#include <mutex>

namespace owl {


    std::mutex frame_lock;
    cv::Mat frame_buff[2];
    cv::Mat* frame = nullptr;

    void frame_thread_func(cv::VideoCapture& vcap) {
        // Switch between frame_buff[0] and frame_buff[1] to avoid copying
        for (int fb = 0;;fb = (fb == 0) ? 1 : 0) {
            cv::Mat tmp_frame;
            // Check for errors when reading the next frame
            if (!vcap.read(frame_buff[fb])) {
                continue;
            }
            else {
                // We have a valid frame. Switch it out.
                frame_lock.lock();
                frame = &frame_buff[fb];
                frame_lock.unlock();
            }

            // Put the thread to sleep for a little while
            std::this_thread::sleep_for(
                std::chrono::milliseconds(10)
            );
        }
    }

    int run(std::string video_url, std::string ip, int port) {
        #if __cplusplus >= 201703L
            auto tmp_conn = Connection::from(ip, port);
            if (!tmp_conn) {
                std::cerr << "Could not create connection to '" << ip << ":" << port << "'." << std::endl;
                return 4;
            }
            Connection conn = std::move(*tmp_conn);
        #else
            Connection conn = Connection::from(ip, port);
        #endif
        conn.start();

        // Attempt to open a video capture
        cv::VideoCapture vcap(video_url);
        if (!vcap.isOpened()) {
            std::cerr << "Could not open video capture at URL '" << video_url << "'." << std::endl;
            return 2;
        }

        // Spawn a thread to collect frames
        std::thread frame_thread(frame_thread_func, std::ref(vcap));

        // PWM parameters
        Params params = Params::centre();

        // Main execution loop
        bool running = true;
        while (running) {

            // Get the current frame, flip it and split it
            frame_lock.lock();
            if (frame != nullptr) { // it's a valid frame
                cv::Mat flipped_frame;
                cv::flip(*frame, flipped_frame, 1);
                frame_lock.unlock();

                cv::Mat cam_frames[2];
                cam_frames[0] = flipped_frame(cv::Rect(0, 0, 640, 480));
                cam_frames[1] = flipped_frame(cv::Rect(640, 0, 640, 480));

                cv::imshow("Left Camera (0)", cam_frames[0]);
                cv::imshow("Right Camera (1)", cam_frames[1]);
            }
            else {
                frame_lock.unlock();
            }

            // Wait for 1 ms or handle any key presses
            const int turn_rate = 10;
            switch (auto key = cv::waitKey(1)) {
                case -1: break;
                case '1': params.mode = Params::Mode::CONTROL; break;
                case '2': params.mode = Params::Mode::SINUSOIDAL; break;
                case '3': params.mode = Params::Mode::STEREO; break;
                case '4': params.mode = Params::Mode::CHAMELEON; break;
                case '5': params.mode = Params::Mode::CRAZY; break;
                case '6': params.mode = Params::Mode::CONFUSED; break;
                case '7': params.mode = Params::Mode::NECK_SINUSOIDAL; break;
                case 'a': {
                    params.eyes[0].x -= turn_rate;
                    params.eyes[1].x -= turn_rate;
                    break;
                }
                case 'd': {
                    params.eyes[0].x += turn_rate;
                    params.eyes[1].x += turn_rate;
                    break;
                }
                case 's': {
                    params.eyes[0].y += turn_rate;
                    params.eyes[1].y -= turn_rate;
                    break;
                }
                case 'w': {
                    params.eyes[0].y -= turn_rate;
                    params.eyes[1].y += turn_rate;
                    break;
                }
                case 'q': params.neck += turn_rate; break;
                case 'e': params.neck -= turn_rate; break;
                case 'x': running = false; break;
                default: {
                    std::cerr << "Unrecognised key '" << key << "' pressed." << std::endl;
                    break;
                }
            };

            // Update PWM parameters
            if (conn.get_params().mode == Params::Mode::CONTROL) {
                conn.set_params(params);
            }
            else {
                Params p= conn.get_params();
                p.mode = params.mode;
                conn.set_params(p);
            }
        }

        // Deinit OpenCV
        cv::destroyAllWindows();

        return 1;
    }
}
