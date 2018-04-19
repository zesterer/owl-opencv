#ifndef OWL_HPP_CORREL
#define OWL_HPP_CORREL

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// Std
#include <utility>

namespace owl {
	struct Correl {
		cv::Point match;
		cv::Mat result;

	private:
		Correl(cv::Point&& m, cv::Mat&& r) :
			match(m),
			result(r)
		{}

	public:
		Correl() {}

		static Correl from_match(cv::Mat& img, cv::Mat& templ, cv::Rect tgt) {
			// Create result matrix
			int
				res_cols = (img.cols - templ.cols) + 1,
				res_rows = (img.rows - templ.rows) + 1;

			// Initiate values
			cv::Point match;
			cv::Mat result;
			result.create(res_rows, res_cols, CV_32FC1);

			// Match and normalise
			int method = CV_TM_CCOEFF_NORMED;
			cv::matchTemplate(img, templ, result, method);
			cv::normalize(result, result, 0, 1, cv::NormTypes::NORM_MINMAX, -1, cv::Mat());

			// Localise the best match
			double min, max;
			cv::Point min_loc, max_loc, match_loc;
			minMaxLoc(result, &min, &max, &min_loc, &max_loc, cv::Mat());

			if (method == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED) {
				return Correl(std::move(min_loc), std::move(result));
			}
			else {
				return Correl(std::move(max_loc), std::move(result));
			}
		}
	};

    struct CorrelData {
        cv::Mat templ;
        cv::Rect rect;
        Correl correl[2];

        CorrelData() : rect(320 - 32, 240 - 32, 64, 64) {}
    };
}

#endif
