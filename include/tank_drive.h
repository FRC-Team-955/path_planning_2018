#pragma once
#include <shared_network_types.h>
#include <opencv2/opencv.hpp>
#include <parametric_output.h>
#include <node.h>

namespace TankDrive {
	struct TankOutput {
		TankDriveMotionUnit motion;
		cv::Point2f left_position;
		cv::Point2f right_position;
		cv::Point2f center_position;
		float robot_direction;
	};
	float evaluate (ParametricOutput parametric, TankOutput& output, float max_velocity, float dt, float wheel_distance);
	struct Traversal {
		std::vector<Node>::iterator node;
		std::vector<Node>::iterator node_end;
		std::vector<TimedAction>::iterator actions;
		float time_s;
		float index;
		float wheel_distance;
		Traversal(std::vector<Node>& nodes, float wheel_distance) : index(0.0), time_s(0.0), wheel_distance(wheel_distance) {
			node = nodes.begin();
			node_end = nodes.end();
		};
		bool next(TankDrive::TankOutput& output, float dt);
	};
}
