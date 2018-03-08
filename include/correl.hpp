#ifndef OWL_HPP_CORREL
#define OWL_HPP_CORREL

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

namespace owl {
    struct Correl {
	private:
		cv::Point match;
		cv::Mat result;

		Correl(cv::Point&& m, cv::Mat&& r) :
		    match(m),
		    result(r)
		{}

	public:
		static Correl from_match(cv::Mat left, cv::Mat right, cv::Mat templ, cv::Mat tgt) {
			// Create result matrix
			int
			    res_cols = (left.cols - templ.cols) + 1,
			    res_rows = (left.rows - templ.rows) + 1;

			// Initiate values
			cv::Mat match;
			match.create(res_rows, res_cols, CV_32FC1);
			cv::Point result;

			// Match and normalise
			int method = CV_TM_CCOEFF_NORMED;
			matchTemplate(left, templ, result, method);
			normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

			// Localise the best match
			double min, max;
			cv::Point min_loc, max_loc, match_loc;
			minMaxLoc(result, &min, &max, &min_loc, &max_loc, Mat());

			if (method == CV_TM_SQDIFF || method == CV_TM_SQDIFF_NORMED) {
				return Correl(min_loc, result);
			}
			else {
				return Correl(max_loc, result);
			}
		}
	};
}

#endif
