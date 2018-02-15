// Local
#include <run.hpp>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Std
#include <iostream>
#include <thread>

namespace owl {
	//void thread() {
	//}

	int run(std::string video_url, std::string pi_addr, int pi_port) {

		// Attempt to open a video capture
		cv::VideoCapture vcap(video_url);
		if (!vcap.isOpened()) {
			std::cerr << "Could not open video capture at URL '" << video_url << "'." << std::endl;
			return 2;
		}

		// Main execution loop
		bool running = true;
		while (running) {
			cv::Mat frame;
			if (!vcap.read(frame)) {
				std::cerr << "Failed to read frame from video capture." << std::endl;
				return 3;
			}

			cv::imshow("Image", frame);
		}

		// Deinit OpenCV
		cv::destroyAllWindows();
		
		return 1;
	}
}
