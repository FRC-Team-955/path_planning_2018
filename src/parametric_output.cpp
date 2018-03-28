#include <parametric_output.h>

float ParametricOutput::velocity_magnitude_xy () {
	return cv::norm(velocity);
}

float ParametricOutput::velocity_magnitude_derivative_xy () {
	return ((acceleration.y * velocity.y) + (acceleration.x * velocity.x)) / velocity_magnitude_xy();
}

float ParametricOutput::direction_xy () {
	return std::atan2(velocity.y, velocity.x);
};

float ParametricOutput::sum_derivative_squares_xy () {
	return powf(velocity.x, 2.0) + powf(velocity.y, 2.0);
}

cv::Point2f ParametricOutput::perpendicular_unit_vector_xy () {
	return NormalTo(velocity) / velocity_magnitude_xy();
}

cv::Point2f ParametricOutput::perpendicular_unit_vector_derivative_xy () {
	return ((NormalTo(acceleration)) * velocity_magnitude_xy() -
			(NormalTo(velocity)) * velocity_magnitude_derivative_xy())
		/ sum_derivative_squares_xy();
}

float ParametricOutput::change_in_slope() {
	return ((acceleration.y * velocity.x) - (acceleration.x * velocity.y)) / powf(velocity.x, 2.0);
}

float ParametricOutput::change_in_angle() {
	return (1.0 / (1.0 + powf(velocity.y / velocity.x, 2.0))) * change_in_slope();
}

cv::Point2f ParametricOutput::NormalTo(cv::Point2f input) {
	return cv::Point2f(-input.y, input.x);
}
