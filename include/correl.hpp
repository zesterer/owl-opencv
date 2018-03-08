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

		static Correl from_match(cv::Mat& left, cv::Mat& right, cv::Mat& templ, cv::Rect tgt) {
			// Create result matrix
			int
				res_cols = (left.cols - templ.cols) + 1,
				res_rows = (left.rows - templ.rows) + 1;

			// Initiate values
			cv::Point match;
			cv::Mat result;
			result.create(res_rows, res_cols, CV_32FC1);

			// Match and normalise
			int method = CV_TM_CCOEFF_NORMED;
			cv::matchTemplate(left, templ, result, method);
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
}

#endif
