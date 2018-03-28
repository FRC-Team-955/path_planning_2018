#pragma once
#include <opencv2/opencv.hpp>
struct ParametricOutput {
	cv::Point2f position; //f(index)
	cv::Point2f velocity; //f'(index)
	cv::Point2f acceleration; //f''(index)

	//The total velocity over dj
	float velocity_magnitude_xy();

	//Derivative of the above (Over dj)
	float velocity_magnitude_derivative_xy ();

	//Angle of the tangent line relative to the origin
	float direction_xy();;

	//x'(j)^2 + y'(j)^2
	float sum_derivative_squares_xy ();

	//Normalized perpendicular vector to the tangent line
	cv::Point2f perpendicular_unit_vector_xy ();

	//The change in position of the unit vector above over dj
	cv::Point2f perpendicular_unit_vector_derivative_xy ();

	//d(slope)/dj
	float change_in_slope();

	//d(tan^-1 (y'(j)/x'(j))) / dj
	float change_in_angle();

	cv::Point2f NormalTo(cv::Point2f input);

};
