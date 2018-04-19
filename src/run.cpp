// Local
#include <run.hpp>
#include <comm.hpp>
#include <correl.hpp>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/videoio.hpp>

// Std
#include <iostream>
#include <thread>
#include <mutex>
#include <algorithm>

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

    // Correlation things
    namespace correl {
        cv::Mat templ;
        cv::Rect rect(320 - 32, 240 - 32, 64, 64);
        Correl correl[2];
    }

    namespace chess {
        cv::Mat ref;
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

        // Main execution loop
        bool running = true;
        while (running) {

            // Get the current frame, flip it and split it
            frame_lock.lock();
            if (frame != nullptr) { // it's a valid frame
                cv::Mat flipped_frame;
                cv::flip(*frame, flipped_frame, 1);
                frame_lock.unlock();

                using Pixel = cv::Point3_<uint8_t>;
                flipped_frame.forEach<Pixel>([](Pixel& pixel, const int* pos) -> void {
                    float diff = std::max(pixel.x, std::max(pixel.y, pixel.z)); //std::max(abs(pixel.x - pixel.y), std::max(abs(pixel.y - pixel.z), abs(pixel.z - pixel.x)));

                    float len = sqrt(
                        pixel.x * pixel.x +
                        pixel.y * pixel.y +
                        pixel.z * pixel.z
                    );
                    float sx = 255 * (pixel.x / len);
                    float sy = 255 * (pixel.y / len);
                    float sz = 255 * (pixel.z / len);

                    pixel.x = std::max(0.f, std::min(255.f, pixel.x + (sx - pixel.x) * (0.1f - diff / 200.f)));
                    pixel.y = std::max(0.f, std::min(255.f, pixel.y + (sy - pixel.y) * (0.1f - diff / 200.f)));
                    pixel.z = std::max(0.f, std::min(255.f, pixel.z + (sz - pixel.z) * (0.1f - diff / 200.f)));
                });

                cv::Mat cam_frames[2];
                cam_frames[0] = flipped_frame(cv::Rect(0, 0, 640, 480));
                cam_frames[1] = flipped_frame(cv::Rect(640, 0, 640, 480));

                // Deal with correlation things
                if (conn.get_params().mode == Params::Mode::CAPTURE) {
                    correl::templ = cam_frames[0](correl::rect);

                    // Now that we've captured the target, switch to correlation mode
                    Params p = conn.get_params();
                    p.mode = Params::Mode::CORRELATION;
                    conn.set_params(p);
                }
                else if (conn.get_params().mode == Params::Mode::CORRELATION) {
                    cv::imshow("Correlation target", correl::templ);

                    // Attempt to match the correlation target
                    for (int i = 0; i < 2; i ++) {
                        correl::correl[i] = Correl::from_match(
                            cam_frames[i],
                            correl::templ,
                            correl::rect
                        );
                    }

                    for (int i = 0; i < 2; i ++) {
                        cv::rectangle(
                            cam_frames[i],
                            correl::correl[i].match,
                            cv::Point(
                                correl::correl[i].match.x + correl::templ.cols,
                                correl::correl[i].match.y + correl::templ.rows
                            ),
                            cv::Scalar(255, 0, 0), 2, 8, 0
                        );
                    }
                } else if (conn.get_params().mode == Params::Mode::CAPTURE_CHESS) {
                    cam_frames[0].copyTo(chess::ref);

                    // Now that we've captured the target, switch to chess calibration mode
                    Params p = conn.get_params();
                    p.mode = Params::Mode::CHESS_CALIB;
                    conn.set_params(p);
                } else if (conn.get_params().mode == Params::Mode::CHESS_CALIB) {
                    std::vector<cv::Point2f> chess_corners;
                    bool found = cv::findChessboardCorners(cam_frames[0], cv::Size(9,6), chess_corners);
                    if (found) {
                        std::vector<cv::Point2f> chess_ref_corners;
                        bool found = cv::findChessboardCorners(chess::ref, cv::Size(9,6), chess_ref_corners);
                        if (found) {
                            for (int i = 0; i < chess_corners.size(); i ++) {
                                auto p = chess_corners[i];
                                cv::rectangle(cam_frames[0], cv::Rect(p.x - 4, p.y - 4, 8, 8), cv::Scalar(0, i * 5, 0), 2, 8, 0);
                            }
                            //cv::drawChessboardCorners(cam_frames[0], cv::Size(9, 6), cv::Mat(chess_corners), true);
                            cv::Mat Hom = cv::findHomography(chess_corners, chess_ref_corners);
                            cv::Mat Planar;
                            cv::warpPerspective(cam_frames[0], Planar, Hom, chess::ref.size());
                            cv::imshow("test",Planar);
                        }
                    }
                }

                // Draw target rectangle
                for (int i = 0; i < 2; i ++) {
                    cv::rectangle(cam_frames[i], correl::rect, cv::Scalar::all(255), 2, 8, 0);
                }

                cv::imshow("Left Camera (0)", cam_frames[0]);
                cv::imshow("Right Camera (1)", cam_frames[1]);
            }
            else {
                frame_lock.unlock();
            }

            Params params = conn.get_params_lock();

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
                case '8': params.mode = Params::Mode::CHESS_CALIB; break;
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
                case 'r': params = Params::centre(); break;
                case 'q': params.neck += turn_rate; break;
                case 'e': params.neck -= turn_rate; break;
                case 'x': running = false; break;
                case 'c': params.mode = Params::Mode::CAPTURE; break;
                case '#': params.mode = Params::Mode::CAPTURE_CHESS; break;
                default: {
                    std::cerr << "Unrecognised key '" << key << "' pressed." << std::endl;
                    break;
                }
            };

            if (params.mode == Params::Mode::CORRELATION) {
                // TODO: Add tracking code here
                for (int i = 0; i < 2; i ++) {
                    params.eyes[i].x += (correl::correl[i].match.x - correl::rect.x) * 0.1;
                    params.eyes[i].y += (i == 0 ? 1 : -1) * (correl::correl[i].match.y - correl::rect.y) * 0.1;
                }
            }

            conn.set_params_lock(params);
        }

        // Deinit OpenCV
        cv::destroyAllWindows();

        return 1;
    }
}
