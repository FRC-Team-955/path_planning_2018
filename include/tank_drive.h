#pragma once
#include <shared_network_types.h>
#include <opencv2/opencv.hpp>
#include <parametric_output.h>

namespace TankDrive {
	struct TankOutput {
		TankDriveMotionUnit motion;
		cv::Point2f left_position;
		cv::Point2f right_position;
		cv::Point2f center_position;
		float robot_direction;
	};
	float evaluate (ParametricOutput parametric, TankOutput& output, float max_velocity, float dt, float wheel_distance);
}
