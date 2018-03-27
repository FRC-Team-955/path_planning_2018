#include <curses.h>
#include <iostream>
#include <string.h>
#include <vector>

// TODO
// Add an inset mode for adding new elements (Somehow, outside of the loop...?!)
// Make this work for the real nodes
// Serialization to a file using JSON or YAML or something
// Make the whole thing a class
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

struct Node {
	char name[256];
	bool is_open;
	std::vector<TimedAction> actions;
	Node copy();
	void menu_action(WINDOW *window, int &current_line, int &selected_h, int selected_line, int input_key);
};

Node Node::copy() {
	Node instance;
	instance.is_open = false;
	return instance;
}

void Node::menu_action(WINDOW *window, int &current_line, int &selected_h, int selected_line, int input_key) {
	is_open ^= input_key == '\n' && selected_line == current_line; //Toggle being expanded with enter key
	wattrset(window, selected_line == current_line ? A_STANDOUT : !A_STANDOUT); //If selected, highlight
	mvwprintw(window, current_line, 1, "[%s] %s", is_open ? "-" : "+", name); //Print the name, and [+] if closed and [-] if open
	current_line++;
	if (is_open) {
		for (auto& action : actions) {
			size_t cursor_offset = 4; //How far from the left we should be
			size_t horizontal_index = 0; //Which option we are on
			wattrset(window, selected_line == current_line ? A_STANDOUT : !A_STANDOUT); //If this option row is selected, highlight it
			mvwprintw(window, current_line, cursor_offset, "%.3f:", action.index); //Print option index to 3 decimal places
			cursor_offset += 7; //Ready the cursor position for the options
			//Dirty macro that sets bitflags really nice. Basically prints enum names, and toggles them on and off based on key input and cursor position
#define PRINT_OPTION(NAME) \
			wattrset(window, A_NORMAL); \
			if (selected_h == horizontal_index && selected_line == current_line) { \
				if (input_key == '\n') (int&)action.action ^= NAME; \
				wattron(window, horizontal_index == selected_h ? A_UNDERLINE : !A_UNDERLINE); \
			} \
			wattron(window, action.action & NAME ? A_STANDOUT : !A_STANDOUT); \
			mvwprintw(window, current_line, cursor_offset, "%s", #NAME); \
			cursor_offset += strlen(#NAME) + 1; \
			horizontal_index++;

			PRINT_OPTION(Up);
			PRINT_OPTION(Down);
			PRINT_OPTION(Intake_In);
			PRINT_OPTION(Intake_Expel);
#undef PRINT_OPTION

			wattroff(window, A_UNDERLINE);
			current_line++;
		}
	}
}

int main() {
	std::vector<Node> nodes;
	//for (int i = 0; i < 3; i++) {
	nodes.push_back({"Node a", false});
	nodes.back().actions.push_back({1.0, Action::Up});
	nodes.back().actions.push_back({0.01, Action::Down});
	nodes.push_back({"Node b", true});
	nodes.back().actions.push_back({0.01, (Action)(Action::Down | Action::Up)});
	//}

	/* Initialize curses */
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	WINDOW *window = newwin(25, 50, 0, 0);

	nodelay(window, true);
	int selected_v = 0;
	int selected_h = 0;
	int input_key;
	while ((input_key = getch()) != 'q') {
		wclear(window);
		box(window, 0, 0);
		switch (input_key) {
			case KEY_UP:
			case 'k':
				if (selected_v > 1)
					selected_v--;
				break;

			case KEY_DOWN:
			case 'j':
				selected_v++;
				break;

			case KEY_LEFT:
			case 'h':
				if (selected_h > 1)
					selected_h--;
				break;

			case KEY_RIGHT:
			case 'l':
				selected_h++;
				break;
		}
		int current_line = 1;
		int thing_idx = 0;
		for (size_t i = 0; i < nodes.size(); i++) {
			nodes[i].menu_action(window, current_line, selected_h, selected_v, input_key);
		}
		wattroff(window, A_STANDOUT);
		wrefresh(window);
		if (selected_v > current_line) {
			selected_v = current_line;
		}
	}

	delwin(window);
	endwin();
}
