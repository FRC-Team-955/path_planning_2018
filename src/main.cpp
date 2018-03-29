#include <iostream>
#include <string.h>
#include <vector>
#include <node_tui.h>
#include <node_gui.h>
#include <opencv2/opencv.hpp>

// TODO
// JSON serialization/deserialization
// Command line arg for file name (turns on tui..?)
//    0 arg = headless + default socket name
//    1 arg = tui edit
//    2 arg = tui edit + custom socket name
// Sockets and real traversal
// Actions during traversal

void save(std::vector<Node>& nodes, char* filename);
void load(std::vector<Node>& nodes, char* filename);

int main(int argc, char** argv) {
	std::vector<Node> nodes;

	if (argc > 0) {
		load(nodes, argv[1]);

		NodeTui tui;
		NodeGui gui (635.0, (char*)"RLR", true);
		while (tui.update(nodes)) {
			gui.update(nodes);
		}

		save(nodes, argv[1]);
	}

}

void save(std::vector<Node>& nodes, char* filename) {
	cv::FileStorage fs;
	if (fs.open(filename, cv::FileStorage::WRITE | cv::FileStorage::FORMAT_YAML)) {
		fs << "nodes" << "[";
		for (auto& node : nodes) {
			fs << "{:";
#define ATTRIB(NAME) fs << #NAME << node.NAME;
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
			for (auto& action : node.actions) {
				fs << "{:" << "time" << action.time << "action" << action.action << "}";
			}
			fs << "]"; //End actions

			fs << "}"; //End node
		}
		fs << "]"; //End nodes
	}
}

void load(std::vector<Node>& nodes, char* filename) {
	cv::FileStorage fs;
	if (fs.open(filename, cv::FileStorage::READ | cv::FileStorage::FORMAT_YAML)) {
		cv::FileNode nodes_fn = fs["nodes"];
		for (cv::FileNodeIterator it = nodes_fn.begin(); it < nodes_fn.end(); ++it) {
			cv::FileNode current_node = *it;
			Node node;
			std::string name;
			current_node["name"] >> name;
			strcpy(node.name, name.c_str());
#define ATTRIB(NAME) current_node[#NAME] >> node.NAME;
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
			cv::FileNode actions_fn = current_node["actions"];
			for (cv::FileNodeIterator action_it = actions_fn.begin(); action_it < actions_fn.end(); ++action_it) {
				node.actions.push_back((TimedAction){(*action_it)["time"], (Action)(int)((*action_it)["action"])});
			}
			nodes.push_back(node);
		}
	}
}
