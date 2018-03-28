#include <iostream>
#include <string.h>
#include <vector>
#include <node_tui.h>
#include <node_gui.h>

// TODO
// Traversal using iterators and time for actions
//       Reverse paths are marked with red (Is a result of traversal)
// JSON serialization/deserialization
// Command line arg for file name (turns on tui..?)
//    0 arg = headless + default socket name
//    1 arg = tui edit
//    2 arg = tui edit + custom socket name
// Sockets and real traversal

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
