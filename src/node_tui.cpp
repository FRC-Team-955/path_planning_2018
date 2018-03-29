#include <node_tui.h>

//TODO: add node name editing through edit_string
//      add timedaction time editing through edit_number
//      sepearate node toggle action

void NodeTui::init() {
	initscr(); //Init screen
	start_color(); //Enable color
	cbreak(); //Stop normal character IO
	noecho(); //Don't show what is typed
	for (int i = 0; i < 10; i++) //Sometimes it doesn't enable right
		keypad(stdscr, true); //Enable keyboard
	curs_set(0); //Down't show the cursor
	nodelay(stdscr, true); //Non-blocking input
	curses_window = newwin(25, 50, 0, 0); //Create a new curses window TODO: Configurable size?
}

bool NodeTui::update(std::vector<Node>& nodes) {
	kbd_input_latest = getch();
	if (kbd_input_latest != ERR) {
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
			case 'a': 
				nodes.push_back(Node());
				break;
		}
		draw_vertical = 0;
		draw_horizontal = 0;
		if (nodes.size() > 0) {
			auto node = nodes.begin();
			do {
				next_line(1, 1);
				if (pressed('d')) {
					nodes.erase(node);
				} else {
					if (nodes.size() > 0) {
						edit_bool(node->is_open, (char*)"[-]", (char*)"[+]");
						edit_string(node->name);

						if (node->is_open) {
							draw_idx_horizontal = 0;
							draw_horizontal = 3;
							next_line(0, 3);
							wattrset(curses_window, am_selected() ? A_STANDOUT : !A_STANDOUT); //If selected, highlight
							mvwprintw(curses_window, draw_vertical, draw_horizontal, "Actions:");
							if (pressed('i')) { //TODO: USE A REAL CONSTRUCTOR!!!
								node->actions.push_back({0.0});
							}
							draw_idx_horizontal++;

							auto action = std::begin(node->actions);
							while (action != std::end(node->actions)) {
								draw_horizontal = 4;
								next_line(4, 7);
								if (pressed('d')) {
									node->actions.erase(action);
								} else {
									if (node->actions.size() > 0) {
										edit_number(action->time);
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
							edit_number(node->linger_time);

							next_line(2, 3);
							string((char*)"speeds: ");
							edit_number(node->speed_in);
							edit_number(node->speed_center);
							edit_number(node->speed_out);

							next_line(1, 3);
							string((char*)"position: ");
							edit_number(node->position.x);
							edit_number(node->position.y);

							next_line(1, 3);
							string((char*)"lengths: ");
							edit_number(node->length_in);
							edit_number(node->length_out);

							next_line(0, 3);
							string((char*)"direction: ");
							edit_number(node->direction);

							next_line(0, 3);
							string((char*)"reverse: ");
							edit_bool(node->reverse, (char*)"true", (char*)"false");
						}
						//TODO: Don't do this every loop
						std::sort(node->actions.begin(), node->actions.end());
					}
					node++;
				}

			} while (node != nodes.end());
		}
		wattrset(curses_window, A_NORMAL);
		wrefresh(curses_window);
	}
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

void NodeTui::edit_number(float& number) {
	char number_buffer[50];
	if (pressed('\n')) {
		wrefresh(curses_window);
		echo(); curs_set(1);
		nodelay(stdscr, false);
		mvgetstr(draw_vertical, draw_horizontal, number_buffer);
		nodelay(stdscr, true);
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
		nodelay(stdscr, false);
		mvgetstr(draw_vertical, draw_horizontal, input);
		nodelay(stdscr, true);
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

void NodeTui::edit_bool(bool& input, char* if_true, char* if_false) {
	input ^= pressed('\n');
	selectable_string((char*)(input ? if_true : if_false));
}

NodeTui::~NodeTui() {
	delwin(curses_window);
	endwin();
}
