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
	int current_line = 1;
	int thing_idx = 0;
	for (auto& node : *nodes) {
		node.is_open ^= kbd_input_latest == '\n' && tui_sel_vertical == current_line; //Toggle being expanded with enter key
		if (tui_sel_vertical == current_line && kbd_input_latest == 'i') {
			nodes->push_back({"New Node"});
		}
		wattrset(tui_curses_window, tui_sel_vertical == current_line ? A_STANDOUT : !A_STANDOUT); //If selected, highlight
		mvwprintw(tui_curses_window, current_line, 1, "[%s] %s", node.is_open ? "-" : "+", node.name); //Print the name, and [+] if closed and [-] if open
		current_line++;
		if (node.is_open) {
			wattrset(tui_curses_window, tui_sel_vertical == current_line ? A_STANDOUT : !A_STANDOUT); //If selected, highlight
			mvwprintw(tui_curses_window, current_line, 4, "Actions:");
			if (kbd_input_latest == 'i' && tui_sel_vertical == current_line) {
				node.actions.push_back({0.0});
			}
			current_line++;
			auto action = std::begin(node.actions);
			while (action != std::end(node.actions)) {
				if (tui_sel_vertical == current_line && kbd_input_latest == 'd') {
					node.actions.erase(action);
				} else {
					if (node.actions.size() > 0) {
						size_t cursor_offset = 8; //How far from the left we should be
						size_t horizontal_index = 0; //Which option we are on

						wattrset(tui_curses_window, tui_sel_vertical == current_line ? A_STANDOUT : !A_STANDOUT); //If this option row is selected, highlight it
						mvwprintw(tui_curses_window, current_line, cursor_offset, "%.3f:", action->index); //Print option index to 3 decimal places
						cursor_offset += 7; //Ready the cursor position for the options

						//Dirty macro that sets bitflags really nice. Basically prints enum names, and toggles them on and off based on key input and cursor position
#define BITFLAG_OPTION(NAME) \
						wattrset(tui_curses_window, A_NORMAL); \
						if (tui_sel_horizontal == horizontal_index && tui_sel_vertical == current_line) { \
							if (kbd_input_latest == '\n') (int&)action->action ^= NAME; \
							wattron(tui_curses_window, horizontal_index == tui_sel_horizontal ? A_UNDERLINE : !A_UNDERLINE); \
						} \
						wattron(tui_curses_window, action->action & NAME ? A_STANDOUT : !A_STANDOUT); \
						mvwprintw(tui_curses_window, current_line, cursor_offset, "%s", #NAME); \
						cursor_offset += strlen(#NAME) + 1; \
						horizontal_index++;

						BITFLAG_OPTION(Up);
						BITFLAG_OPTION(Down);
						BITFLAG_OPTION(Intake_In);
						BITFLAG_OPTION(Intake_Expel);
#undef BITFLAG_OPTION
					}
					++action;
				}
				current_line++;
			}

			char number_buffer[50];
#define FLOAT_OPTION(NAME) \
			if (kbd_input_latest == '\n' && tui_sel_vertical == current_line) { \
				mvwprintw(tui_curses_window, current_line, 4, "%s: ", #NAME); \
				wrefresh(tui_curses_window); \
				echo(); curs_set(1); \
				mvgetstr(current_line, strlen(#NAME) + 6, number_buffer); \
				noecho(); curs_set(0); \
				node.NAME = atof(number_buffer); \
			} \
			wattrset(tui_curses_window, tui_sel_vertical == current_line ? A_STANDOUT : !A_STANDOUT); \
			mvwprintw(tui_curses_window, current_line, 4, "%s: %.3f", #NAME, node.NAME); \
			current_line++;

			FLOAT_OPTION(linger_time);
			FLOAT_OPTION(speed_in);
			FLOAT_OPTION(speed_out);
			FLOAT_OPTION(speed_center);
#undef FLOAT_OPTION

		}
	}
	wattroff(tui_curses_window, A_STANDOUT);
	wrefresh(tui_curses_window);
	return true;
}

bool PathEditor::update_gui() {
	return true;
}

PathEditor::~PathEditor() {
	delwin(tui_curses_window);
	endwin();
}
