#pragma once
#include <node.h>
#include <opencv2/opencv.hpp>
#include <parametric_output.h>
#include <shared_network_types.h>

namespace TankDrive {
	struct TankOutput {
		TankDriveMotionUnit motion;
		cv::Point2f left_position;
		cv::Point2f right_position;
		cv::Point2f center_position;
		float robot_direction;
	};
	float evaluate(ParametricOutput parametric, TankOutput &output,
			float max_velocity, float dt, float wheel_distance);
	struct Traversal {
		std::vector<Node>::iterator begin;
		std::vector<Node>::iterator current_node;
		std::vector<Node>::iterator next_node;
		std::vector<Node>::iterator end_node;
		std::vector<TimedAction>::iterator actions;
		float time_s;
		float index;
		float wheel_distance;
		Traversal(std::vector<Node>::iterator begin, std::vector<Node>::iterator end,
				float wheel_distance)
			: index(0.0), time_s(0.0), wheel_distance(wheel_distance), begin(begin),
			end_node(end - 1) {
				reset();
			};
		void reset();
		bool next(TankDrive::TankOutput &output, float dt);
	};
} // namespace TankDrive
