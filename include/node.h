#pragma once
#include <vector>

#undef OK
#include <opencv2/opencv.hpp>
#include <parametric_output.h>
#include <spline.h>

enum Action {
	Up = 1,
	Down = 2,
	Intake_Expel = 4,
	Intake_In = 8,
};

struct TimedAction {
	float time;
	Action action;
};

class Node {
	public:
		Node();
		char name[256];
		cv::Point2f position;
		float speed_in;
		float speed_center;
		float speed_out;
		float length_in;
		float length_out;
		bool is_open;
		bool reverse;
		float linger_time;
		float direction;
		std::vector<TimedAction> actions;
		cv::Point2f get_in_ctrlpt();
		cv::Point2f get_out_ctrlpt();
		cv::Point2f get_center_ctrlpt();
		void set_in_ctrlpt(cv::Point2f new_position);
		void set_out_ctrlpt(cv::Point2f new_position);
		void set_center_ctrlpt(cv::Point2f new_position);
		void set_closest_component(cv::Point2f new_position);
		float get_distance_to_closest_component(cv::Point2f input_position);
		ParametricOutput spline(Node* node, float j);
		float speed_ramp(Node* node, float j);
		// TODO:
		// void serialize(JsonNode& node)
		// Node(JsonNode& node)
};
