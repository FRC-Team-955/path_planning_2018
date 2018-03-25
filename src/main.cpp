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
};

struct Node {
	char name[256];
	bool is_open;
	Action action;
	Node copy();
	void display(WINDOW *window, int &line, int selected_line);
};

Node Node::copy() {
	Node instance;
	instance.is_open = false;
	return instance;
}

void Node::display(WINDOW *window, int &line, int selected_line) {
	mvwprintw(window, line++, 1, "[%s] %s", is_open ? "-" : "+", name);
	wattroff(window, A_STANDOUT);
	if (is_open) {
		mvwprintw(window, line++, 3, "Actions: %d", action);
		char strbuf[100];
	}
}

int main() {
	std::vector<Node> things;
	things.push_back({"Dank memes", false});
	things.push_back({"Sweet dreams", true});

	/* Initialize curses */
	initscr();
	start_color();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(0);

	WINDOW *window = newwin(25, 50, 0, 0);

	nodelay(window, true);
	int selected = 0;
	int input_key;
	while ((input_key = getch()) != 'q') {
		wclear(window);
		box(window, 0, 0);
		switch (input_key) {
			case KEY_UP:
			case 'k':
				if (selected > 0)
					selected--;
				break;
			case KEY_DOWN:
			case 'j':
				if (selected < things.size() - 1)
					selected++;
				break;
		}
		int line = 1;
		int thing_idx = 0;
		for (std::vector<Node>::iterator it = things.begin();
				it != things.end();) {
			if (selected == thing_idx) {
				wattron(window, A_STANDOUT);
				switch (input_key) {
					case '\n':
						it->is_open = !it->is_open;
						break;
					case 'x':
						things.erase(it);
						it--;
						break;
				};
			} else {
				wattroff(window, A_STANDOUT);
			}
			if (things.size() > 0) {
				it->display(window, line, selected);
			}
			it++;
			thing_idx++;
		}
		wattroff(window, A_STANDOUT);
		wrefresh(window);
	}

	delwin(window);
	endwin();
}
