#include <node_tui.h>

//TODO: add node name editing through edit_string
//      add timedaction time editing through edit_number
//      sepearate node toggle action

void NodeTui::init() {
	initscr(); //Init screen
	start_color(); //Enable color
	cbreak(); //Stop normal character IO
	noecho(); //Don't show what is typed
	keypad(stdscr, TRUE); //Enable keyboard
	curs_set(0); //Down't show the cursor
	//nodelay(stdscr, true); //Non-blocking input
	curses_window = newwin(25, 50, 0, 0); //Create a new curses window TODO: Configurable size?
}

bool NodeTui::update() {
	kbd_input_latest = getch();
	wclear(curses_window);
	box(curses_window, 0, 0);
	switch (kbd_input_latest) {
		case KEY_UP: //TODO: Make sure those work in the GLUT keyboard func
		case 'k':
			if (sel_vertical > 1)
				sel_vertical--;
			break;

		case KEY_DOWN:
		case 'j':
			sel_vertical++;
			break;

		case KEY_LEFT:
		case 'h':
			if (sel_horizontal > 0)
				sel_horizontal--;
			break;

		case KEY_RIGHT:
		case 'l':
			sel_horizontal++;
			break;
		case KEY_RESIZE: //TODO: impl!
			break;
	}
	draw_vertical = 0;
	draw_horizontal = 0;
	for (auto& node : *nodes) {
		next_line(0, 0);
		if (pressed('i')) {
			nodes->push_back({"New Node"});
		}
		edit_node(&node);
		if (node.is_open) {
			draw_idx_horizontal = 0;
			draw_horizontal = 3;
			next_line(0, 3);
			wattrset(curses_window, am_selected() ? A_STANDOUT : !A_STANDOUT); //If selected, highlight
			mvwprintw(curses_window, draw_vertical, draw_horizontal, "Actions:");
			if (pressed('i')) {
				node.actions.push_back({0.0});
			}
			draw_idx_horizontal++;

			auto action = std::begin(node.actions);
			while (action != std::end(node.actions)) {
				draw_horizontal = 4;
				next_line(4, 7);
				if (pressed('d')) {
					node.actions.erase(action);
				} else {
					if (node.actions.size() > 0) {
						edit_number(action->index);
						edit_bitflag((char*)"Up", Action::Up, (int&)action->action);
						edit_bitflag((char*)"Down", Action::Down, (int&)action->action);
						edit_bitflag((char*)"Expel", Action::Intake_Expel, (int&)action->action);
						edit_bitflag((char*)"In", Action::Intake_In, (int&)action->action);
					}
					++action;
				}
			}
			draw_horizontal = 3;
			next_line(0, 3);
			string((char*)"linger: ");
			edit_number(node.linger_time);

			next_line(2, 3);
			string((char*)"speeds: ");
			edit_number(node.speed_in);
			edit_number(node.speed_center);
			edit_number(node.speed_out);
		}
	}
	wattrset(curses_window, A_NORMAL);
	wrefresh(curses_window);
	return kbd_input_latest != 'q';
}

bool NodeTui::am_selected() {
	return sel_horizontal == draw_idx_horizontal && sel_vertical == draw_vertical;
}

void NodeTui::next_line(int items, int indent) {
	wattrset(curses_window, A_NORMAL);
	draw_vertical++;
	draw_horizontal = indent;
	draw_idx_horizontal = 0;
	if (sel_horizontal > items && sel_vertical == draw_vertical) {
		sel_horizontal = items;
	}
}

bool NodeTui::pressed(int keycode) {
	return kbd_input_latest == keycode && am_selected();
}

void NodeTui::edit_node(Node* node) {
	node->is_open ^= pressed('\n'); //Toggle being expanded with enter key
	wattrset(curses_window, am_selected() ? A_STANDOUT : !A_STANDOUT); //If selected, highlight
	mvwprintw(curses_window, draw_vertical, 1, "[%s] %s", node->is_open ? "-" : "+", node->name); //Print the name, and [+] if closed and [-] if open
}

void NodeTui::edit_number(float& number) {
	char number_buffer[50];
	if (pressed('\n')) {
		wrefresh(curses_window);
		echo(); curs_set(1);
		mvgetstr(draw_vertical, draw_horizontal, number_buffer);
		noecho(); curs_set(0); \
			number = atof(number_buffer);
	}
	wattrset(curses_window, am_selected() ? A_STANDOUT : !A_STANDOUT);
	sprintf(number_buffer, "%.3f", number);
	mvwprintw(curses_window, draw_vertical, draw_horizontal, "%s", number_buffer);
	draw_horizontal += strlen(number_buffer) + 1;
	draw_idx_horizontal++;
}

void NodeTui::edit_string(char* input) {
	if (pressed('\n')) {
		wrefresh(curses_window);
		echo(); curs_set(1);
		mvgetstr(draw_vertical, draw_horizontal, input);
		noecho(); curs_set(0); \
	}
	selectable_string(input);
}

void NodeTui::selectable_string(char* input) {
	wattrset(curses_window, am_selected() ? A_STANDOUT : !A_STANDOUT);
	string(input);
	draw_idx_horizontal++;
}

void NodeTui::string(char* input) {
	mvwprintw(curses_window, draw_vertical, draw_horizontal, "%s", input);
	draw_horizontal += strlen(input) + 1;
}

void NodeTui::edit_bitflag(char* name, int flag, int& flagedit) {
	wattrset(curses_window, A_NORMAL);
	if (am_selected()) {
		if (kbd_input_latest == '\n') flagedit ^= flag;
		wattron(curses_window, am_selected() ? A_UNDERLINE : !A_UNDERLINE);
	}
	wattron(curses_window, flagedit & flag ? A_STANDOUT : !A_STANDOUT);
	mvwprintw(curses_window, draw_vertical, draw_horizontal, "%s", name);
	draw_horizontal += strlen(name) + 1;
	draw_idx_horizontal++;
}

NodeTui::~NodeTui() {
	delwin(curses_window);
	endwin();
}
