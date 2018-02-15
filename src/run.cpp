// Local
#include <run.hpp>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Std
#include <iostream>
#include <thread>
#include <mutex>

namespace owl {
	struct Params {
		struct { float x = 0.0f, y = 0.0f; } eye[2];
		float neck = 0.0f;
	};

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

	int run(std::string video_url, std::string pi_addr, int pi_port) {
		(void)pi_addr;
		(void)pi_port;

		// Attempt to open a video capture
		cv::VideoCapture vcap(video_url);
		if (!vcap.isOpened()) {
			std::cerr << "Could not open video capture at URL '" << video_url << "'." << std::endl;
			return 2;
		}

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

				cv::Mat cam_frames[2];
				cam_frames[0] = flipped_frame(cv::Rect(0, 0, 640, 480));
				cam_frames[1] = flipped_frame(cv::Rect(640, 0, 640, 480));

				cv::imshow("Left Camera (0)", cam_frames[0]);
				cv::imshow("Right Camera (1)", cam_frames[1]);
			}
			else {
				frame_lock.unlock();
			}

			// Wait for 1 ms or until a key is pressed
			switch (auto key = cv::waitKey(1)) {
				case 'q': running = false; break;
				default: {
					std::cerr << "Unrecognised key '" << key << "' pressed." << std::endl;
					break;
				}
			};
		}

		// Deinit OpenCV
		cv::destroyAllWindows();
		
		return 1;
	}
}
