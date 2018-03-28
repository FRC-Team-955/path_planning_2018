#include <iostream>
#include <string.h>
#include <vector>
#include <node_tui.h>
#include <node_gui.h>

// TODO
// Reverse paths
// Traversal using iterators and time for actions
// JSON serialization/deserialization
// Command line arg for file name (turns on tui..?)


int main() {
	std::vector<Node> nodes;
	nodes.push_back(Node());
	nodes.push_back(Node());

	NodeTui tui;
	NodeGui gui (635.0, (char*)"RLR", true);
	while (tui.update(nodes)) {
		gui.update(nodes);
	}
}
