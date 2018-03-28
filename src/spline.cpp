#include <spline.h>
#include <parametric_output.h>

inline cv::Point2f spline(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d, float t) {
  return (powf(1 - t, 3.0) * a) + (3.0 * powf(1 - t, 2.0) * t * b) +
         (3.0 * (1 - t) * powf(t, 2.0) * c) + (powf(t, 3.0) * d);
}

inline cv::Point2f spline_deriv(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d,
                            float t) {
  return (3.0 * powf(1 - t, 2.0) * (b - a)) + (6.0 * (1 - t) * t * (c - b)) +
         (3.0 * powf(t, 2.0) * (d - c));
}

inline cv::Point2f spline_deriv_sq(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d,
                               float t) {
  return (6.0 * (1 - t) * (c - (2.0 * b) + a)) +
         (6.0 * t * (d - (2.0 * c) + b));
}

inline ParametricOutput spline_par(cv::Point2f a, cv::Point2f b, cv::Point2f c, cv::Point2f d, float t) {
	return {spline(a,b,c,d,t), spline_deriv(a,b,c,d,t), spline_deriv_sq(a,b,c,d,t)};
}

inline float ParametricOutput::spline_f(float a, float b, float c, float d, float t) {
	return (powf(1 - t, 3.0) * a) + (3.0 * powf(1 - t, 2.0) * t * b) +
		(3.0 * (1 - t) * powf(t, 2.0) * c) + (powf(t, 3.0) * d);
}
