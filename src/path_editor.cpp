#include <path_editor.h>

void PathEditor::init() {
	init_tui();
	init_gui();
}

void PathEditor::init_tui() {
	initscr(); //Init screen
	start_color(); //Enable color
	cbreak(); //Stop normal character IO
	noecho(); //Don't show what is typed
	keypad(stdscr, TRUE); //Enable keyboard
	curs_set(0); //Down't show the cursor
	//nodelay(stdscr, true); //Non-blocking input
	tui_curses_window = newwin(25, 50, 0, 0); //Create a new curses window TODO: Configurable size?
}

void PathEditor::init_gui() {
}

bool PathEditor::update() {
	//TODO: Add glut keyboard function at the same time to mirror the getch()
	//      if they both have input, prefer the TUI... Maybe.
	kbd_input_latest = getch();
	return update_tui() && update_gui() && kbd_input_latest != 'q';
}

bool PathEditor::update_tui() {
	wclear(tui_curses_window);
	box(tui_curses_window, 0, 0);
	switch (kbd_input_latest) {
		case KEY_UP: //TODO: Make sure those work in the GLUT keyboard func
		case 'k':
			if (tui_sel_vertical > 1)
				tui_sel_vertical--;
			break;

		case KEY_DOWN:
		case 'j':
			tui_sel_vertical++;
			break;

		case KEY_LEFT:
		case 'h':
			if (tui_sel_horizontal > 0)
				tui_sel_horizontal--;
			break;

		case KEY_RIGHT:
		case 'l':
			tui_sel_horizontal++;
			break;
		case KEY_RESIZE: //TODO: impl!
			break;
	}
	tui_draw_vertical = 0;
	tui_draw_horizontal = 0;
	tui_next_line(0);
	for (auto& node : *nodes) {
		if (tui_pressed('i')) {
			nodes->push_back({"New Node"});
		}
		tui_edit_node(&node);
		tui_next_line(0);
		if (node.is_open) {
			tui_draw_idx_horizontal = 0;
			tui_draw_horizontal = 3;
			wattrset(tui_curses_window, tui_am_selected() ? A_STANDOUT : !A_STANDOUT); //If selected, highlight
			mvwprintw(tui_curses_window, tui_draw_vertical, tui_draw_horizontal, "Actions:");
			if (tui_pressed('i')) {
				node.actions.push_back({0.0});
			}
			tui_draw_idx_horizontal++;

			auto action = std::begin(node.actions);
			while (action != std::end(node.actions)) {
				tui_draw_horizontal = 4;
				tui_next_line(5);
				if (tui_pressed('d')) {
					node.actions.erase(action);
				} else {
					if (node.actions.size() > 0) {
						wattrset(tui_curses_window, tui_am_selected() ? A_STANDOUT : !A_STANDOUT); //If this option row is selected, highlight it
						mvwprintw(tui_curses_window, tui_draw_vertical, tui_draw_horizontal, "%.3f:", action->index); //Print option index to 3 decimal places
						tui_draw_idx_horizontal++;
						tui_draw_horizontal += 7; //Ready the cursor position for the options
						tui_edit_bitflag((char*)"Up", Action::Up, (int&)action->action);
						tui_edit_bitflag((char*)"Down", Action::Down, (int&)action->action);
						tui_edit_bitflag((char*)"Expel", Action::Intake_Expel, (int&)action->action);
						tui_edit_bitflag((char*)"In", Action::Intake_In, (int&)action->action);
					}
					++action;
				}
			}
			tui_draw_horizontal = 3;
			tui_next_line(0);
			tui_edit_number((char*)"linger", node.linger_time);
			tui_next_line(0);
			tui_edit_number((char*)"in", node.speed_in);
			tui_next_line(0);
			tui_edit_number((char*)"out", node.speed_out);
		}
	}
	wattrset(tui_curses_window, A_NORMAL);
	wrefresh(tui_curses_window);
	return true;
}

bool PathEditor::tui_am_selected() {
	return tui_sel_horizontal == tui_draw_idx_horizontal && tui_sel_vertical == tui_draw_vertical;
}

void PathEditor::tui_next_line(int items) {
	tui_draw_vertical++;
	tui_draw_idx_horizontal = 0;
	if (tui_sel_horizontal > items && tui_sel_vertical == tui_draw_vertical) {
		tui_sel_horizontal = items;
	}
}

bool PathEditor::tui_pressed(int keycode) {
	return kbd_input_latest == keycode && tui_am_selected();
}

void PathEditor::tui_edit_node(Node* node) {
	node->is_open ^= tui_pressed('\n'); //Toggle being expanded with enter key
	wattrset(tui_curses_window, tui_am_selected() ? A_STANDOUT : !A_STANDOUT); //If selected, highlight
	mvwprintw(tui_curses_window, tui_draw_vertical, 1, "[%s] %s", node->is_open ? "-" : "+", node->name); //Print the name, and [+] if closed and [-] if open
}

void PathEditor::tui_edit_number(char* name, float& number) {
	if (tui_pressed('\n')) {
		char number_buffer[50];
		mvwprintw(tui_curses_window, tui_draw_vertical, 4, "%s: ", name);
		wrefresh(tui_curses_window);
		echo(); curs_set(1);
		mvgetstr(tui_draw_vertical, strlen(name) + 6, number_buffer);
		noecho(); curs_set(0); \
			number = atof(number_buffer);
	}
	wattrset(tui_curses_window, tui_am_selected() ? A_STANDOUT : !A_STANDOUT);
	mvwprintw(tui_curses_window, tui_draw_vertical, 4, "%s: %.3f", name, number);
	tui_draw_horizontal += 7;
	tui_draw_idx_horizontal++;
}

void PathEditor::tui_edit_bitflag(char* name, int flag, int& flagedit) {
	wattrset(tui_curses_window, A_NORMAL);
	if (tui_am_selected()) {
		if (kbd_input_latest == '\n') flagedit ^= flag;
		wattron(tui_curses_window, tui_am_selected() ? A_UNDERLINE : !A_UNDERLINE);
	}
	wattron(tui_curses_window, flagedit & flag ? A_STANDOUT : !A_STANDOUT);
	mvwprintw(tui_curses_window, tui_draw_vertical, tui_draw_horizontal, "%s", name);
	tui_draw_horizontal += strlen(name) + 1;
	tui_draw_idx_horizontal++;
}

bool PathEditor::update_gui() {
	return true;
}

PathEditor::~PathEditor() {
	delwin(tui_curses_window);
	endwin();
}
