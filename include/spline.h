#pragma once
#include <opencv2/opencv.hpp>
#include <parametric_output.h>

namespace Spline {
	float spline_f(float a, float b, float c, float d, float t);
	cv::Point2f spline(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d, float t);
	cv::Point2f spline_deriv(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d, float t);
	cv::Point2f spline_deriv_sq(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d, float t);
	ParametricOutput spline_par(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d, float t);
}
