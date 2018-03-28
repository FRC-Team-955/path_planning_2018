#include <iostream>
#include <string.h>
#include <vector>
#include <node_tui.h>
#include <node_gui.h>

// TODO
// JSON serialization/deserialization
// OpenGL/GLUT interface
// Command line arg for file name (turns on tui..?)
// Actually put the field and robot tank drive equations in
// REAL TIME EDITING!!!!!!!
// Nonblocking input


int main() {
	std::vector<Node> nodes;
	nodes.push_back(Node());
	nodes.push_back(Node());

	NodeTui tui;
	NodeGui gui;
	while (tui.update(nodes)) {
		gui.update(nodes);
	}
}
