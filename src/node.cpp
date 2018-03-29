#include <node.h>

Node::Node() {
	strcpy(name, "New Node");
	position = cv::Point2f(100.0, 100.0);
	speed_in = 1.0;
	speed_center = 1.0;
	speed_out = 1.0;
	length_in = 100.0;
	length_out = 100.0;
	is_open = true;
	reverse = false;
	linger_time = 0.0;
	float direction = M_PI / 2.0;
}

Node::Node(cv::FileNode input) {
	std::string string_name;
	input["name"] >> string_name;
	strcpy(name, string_name.c_str());
#define ATTRIB(NAME) input[#NAME] >> NAME;
	ATTRIB(position);
	ATTRIB(speed_in);
	ATTRIB(speed_out);
	ATTRIB(speed_center);
	ATTRIB(length_in);
	ATTRIB(length_out);
	ATTRIB(is_open);
	ATTRIB(reverse);
	ATTRIB(linger_time);
	ATTRIB(direction);
#undef ATTRIB
	cv::FileNode actions_fn = input["actions"];
	for (cv::FileNodeIterator action_it = actions_fn.begin(); action_it < actions_fn.end(); ++action_it) {
		actions.push_back((TimedAction){(*action_it)["time"], (Action)(int)((*action_it)["action"])});
	}
}

void Node::save_to( cv::FileStorage &fs ) {
	fs << "{:";
#define ATTRIB(NAME) fs << #NAME << NAME;
	ATTRIB(name);
	ATTRIB(position);
	ATTRIB(speed_in);
	ATTRIB(speed_out);
	ATTRIB(speed_center);
	ATTRIB(length_in);
	ATTRIB(length_out);
	ATTRIB(is_open);
	ATTRIB(reverse);
	ATTRIB(linger_time);
	ATTRIB(direction);
#undef ATTRIB
	fs << "actions" << "[:";
	for (auto& action : actions) {
		fs << "{:" << "time" << action.time << "action" << action.action << "}";
	}
	fs << "]"; 

	fs << "}";
}

cv::Point2f Node::get_out_ctrlpt() {
	return position + (cv::Point2f(cos(direction), sin(direction)) * length_out);
}

cv::Point2f Node::get_in_ctrlpt() {
	return position +
		(cv::Point2f(cos(direction + CV_PI), sin(direction + CV_PI)) *
		 length_in);
}

cv::Point2f Node::get_center_ctrlpt() { return position; }

void Node::set_out_ctrlpt(cv::Point2f new_position) {
	cv::Point2f position_diff = new_position - position;
	direction = atan2(position_diff.y, position_diff.x);
	length_out = cv::norm(position_diff);
}

void Node::set_in_ctrlpt(cv::Point2f new_position) {
	cv::Point2f position_diff = new_position - position;
	direction = atan2(position_diff.y, position_diff.x) + CV_PI;
	length_in = cv::norm(position_diff);
}

void Node::set_center_ctrlpt(cv::Point2f new_position) {
	position = new_position;
}

void Node::set_closest_component(cv::Point2f new_position) {
	float dist_out = cv::norm(get_out_ctrlpt() - new_position);
	float dist_in = cv::norm(get_in_ctrlpt() - new_position);
	float dist_cent = cv::norm(get_center_ctrlpt() - new_position);
	if (dist_out < dist_in && dist_out < dist_cent) {
		set_out_ctrlpt(new_position);
	} else if (dist_in < dist_out && dist_in < dist_cent) {
		set_in_ctrlpt(new_position);
	} else {
		set_center_ctrlpt(new_position);
	}
}

float Node::get_distance_to_closest_component(cv::Point2f input_position) {
	return std::min(std::min(cv::norm(get_out_ctrlpt() - input_position),
				cv::norm(get_in_ctrlpt() - input_position)),
			cv::norm(get_center_ctrlpt() - input_position));
}

float Node::speed_ramp(std::vector<Node>::iterator other, float j) {
	if (reverse) {
		return Spline::spline_f(
				other->speed_center, 
				other->speed_out,
				this->speed_in,
				this->speed_center,
				j);
	} else {
		return Spline::spline_f(
				this->speed_center,
				this->speed_out,
				other->speed_in,
				other->speed_center, j);
	}
}

ParametricOutput Node::spline(std::vector<Node>::iterator other, float j) {
	if (reverse) {
		return Spline::spline_par(
				other->get_center_ctrlpt(), 
				other->get_out_ctrlpt(),
				this->get_in_ctrlpt(),
				this->get_center_ctrlpt(), 
				j);
	} else {
		return Spline::spline_par(
				this->get_center_ctrlpt(), 
				this->get_out_ctrlpt(),
				other->get_in_ctrlpt(),
				other->get_center_ctrlpt(), j);
	}
}

