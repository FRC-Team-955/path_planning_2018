#pragma once
#include <vector>

enum Action {
	Up = 1,
	Down = 2,
	Intake_Expel = 4,
	Intake_In = 8,
};

struct TimedAction {
	float index;
	Action action;
};

class Node {
	public:
		char name[256];
		bool is_open;
		std::vector<TimedAction> actions;
		float linger_time;
		float speed_in;
		float speed_center;
		float speed_out;
		// Point2f spline(Node* node, float j);
		// void serialize(JsonNode& node)
		// Node(JsonNode& node)
	private:
};
