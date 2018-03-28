#pragma once
#include <opencv2/opencv.hpp>

inline float spline_f(float a, float b, float c, float d, float t);
inline cv::Point2f spline(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d, float t);
inline cv::Point2f spline_deriv(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d, float t);
inline cv::Point2f spline_deriv_sq(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d, float t);
inline ParametricOutput spline_par(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d, float t);
