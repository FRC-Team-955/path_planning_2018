#include <iostream>
#include <string.h>
#include <vector>
#include <path_editor.h>

// TODO
// Add an inset mode for adding new elements (Somehow, outside of the loop...?!)
// Make this work for the real nodes
// Serialization to a file using JSON or YAML or something
// Make the whole thing a class


int main() {
	std::vector<Node> nodes;
	nodes.push_back({"Node a", false});
	nodes.back().actions.push_back({1.0, Action::Up});
	nodes.back().actions.push_back({0.01, Action::Down});
	nodes.push_back({"Node b", true});
	nodes.back().actions.push_back({0.01, (Action)(Action::Down | Action::Up)});

	PathEditor editor (&nodes);
	while (editor.update());

}
