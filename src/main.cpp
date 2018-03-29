#include <iostream>
#include <string.h>
#include <vector>
#include <chrono>
#include <node_tui.h>
#include <node_gui.h>
#include <socket.h>
#include <opencv2/opencv.hpp>
#include <shared_network_types.h>

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

std::chrono::high_resolution_clock::time_point t1;
std::chrono::high_resolution_clock::time_point t2;

int main(int argc, char** argv) {
	std::vector<Node> nodes;

	RioCommand output_command;
	JetsonCommand setup_info;
	JetsonCommand input_command;

	SocketServer* rio;
	
	bool ui_enable = argc >= 2;
	bool socket_enable = argc < 2;
	if (argc >= 3) {
		ui_enable = true;
		socket_enable = true;
	}

	if (socket_enable) {
 		rio = new SocketServer (5801);
	}
	
	std::cout << "Waiting for setup" << std::endl;
	if (ui_enable && !socket_enable) {
		strncpy(setup_info.config, argv[1], 3);
	} else {
		setup_info.type = JetsonCommand::Type::JetsonNone;
		while (setup_info.type != JetsonCommand::Type::Reset) {
			//TODO: Recoverable reboots, queries where the bot thinks it is and sets self to that position
			output_command.type = RioCommand::Type::Request_Setup;
			rio->write_to(&output_command, sizeof(RioCommand));
			rio->read_to(&setup_info, sizeof(JetsonCommand)); 
		}
	}
	std::cout << "Got setup info" << std::endl;
	char filename[50];
	strncpy(filename, setup_info.config, 3);
	strcat(filename, ".yml");
	load(nodes, filename);

	TankDrive::Traversal trav (nodes.begin(), nodes.end(), 635.0 / 2.0);
	TankDrive::TankOutput tank_output;
	Action action_output;
	NodeTui* tui;
	NodeGui* gui;
	if (ui_enable) {
		gui = new NodeGui(635.0 / 2.0, setup_info.config, true);
		tui = new NodeTui();
	}

	int lastsize = nodes.size();
	t1 = std::chrono::high_resolution_clock::now();
	float ms = 10.0;
	while (true) {
		if (socket_enable) {
			if (!trav.next(tank_output, action_output, ms)) {
				trav.reset();
				output_command.type = RioCommand::Type::Stop;
				std::cerr << "SENDING STOP" << std::endl;
			} else {
				output_command.type = RioCommand::Type::Motion;
			}
			if (lastsize != nodes.size()) {
				trav = TankDrive::Traversal(nodes.begin(), nodes.end(), 635.0 / 2.0);
			}
		}
		lastsize = nodes.size();
		if (ui_enable) {
			gui->update(nodes, tank_output);
			if (!tui->update(nodes)) {
				break;
			}
		}
		if (socket_enable) {
			output_command.motion = tank_output.motion;
			output_command.action = action_output;
			rio->write_to(&output_command, sizeof(output_command));
			rio->read_to(&input_command, sizeof(input_command)); 
		}
		t2 = std::chrono::high_resolution_clock::now();
		ms = std::abs(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count());
		t1 = t2;
	}
	if (ui_enable) {
		tui->~NodeTui();
		delete gui;
		delete tui;
	}
	if (socket_enable) {
		delete rio;
	}
	save(nodes, filename);
}

void save(std::vector<Node>& nodes, char* filename) {
	cv::FileStorage fs;
	if (fs.open(filename, cv::FileStorage::WRITE | cv::FileStorage::FORMAT_YAML)) {
		fs << "nodes" << "[";
		for (auto& node : nodes) {
			node.save_to(fs);
		}
		fs << "]";
	}
}

void load(std::vector<Node>& nodes, char* filename) {
	cv::FileStorage fs;
	if (fs.open(filename, cv::FileStorage::READ | cv::FileStorage::FORMAT_YAML)) {
		cv::FileNode nodes_fn = fs["nodes"];
		for (cv::FileNodeIterator it = nodes_fn.begin(); it < nodes_fn.end(); ++it) {
			nodes.push_back(Node(*it));
		}
	}
}
