#include <iostream>
#include <string.h>
#include <vector>
#include <node_tui.h>

// TODO
// JSON serialization/deserialization
// OpenGL/GLUT interface
// Command line arg for file name (turns on tui..?)
// Actually put the field and robot tank drive equations in
// REAL TIME EDITING!!!!!!!
// Nonblocking input


int main() {
	std::vector<Node> nodes;
	nodes.push_back({"Node a", false});
	nodes.back().actions.push_back({1.0, Action::Up});
	nodes.back().actions.push_back({0.01, Action::Down});
	nodes.push_back({"Node b", true});
	nodes.back().actions.push_back({0.01, (Action)(Action::Down | Action::Up)});

	NodeTui tui (&nodes);
	while (tui.update());

}
