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
	tui_draw_vertical = 1;
	for (auto& node : *nodes) {
		tui_edit_node(&node);
		if (tui_sel_vertical == tui_draw_vertical && kbd_input_latest == 'i') {
			nodes->push_back({"New Node"});
		}
		if (node.is_open) {
			wattrset(tui_curses_window, tui_sel_vertical == tui_draw_vertical ? A_STANDOUT : !A_STANDOUT); //If selected, highlight
			mvwprintw(tui_curses_window, tui_draw_vertical, 4, "Actions:");
			if (kbd_input_latest == 'i' && tui_sel_vertical == tui_draw_vertical) {
				node.actions.push_back({0.0});
			}
			tui_draw_vertical++;
			auto action = std::begin(node.actions);
			while (action != std::end(node.actions)) {
				if (tui_sel_vertical == tui_draw_vertical && kbd_input_latest == 'd') {
					node.actions.erase(action);
				} else {
					if (node.actions.size() > 0) {
						tui_draw_horizontal = 8; //How far from the left we should be
						int horizontal_index = 0; //Which option we are on

						wattrset(tui_curses_window, tui_sel_vertical == tui_draw_vertical ? A_STANDOUT : !A_STANDOUT); //If this option row is selected, highlight it
						mvwprintw(tui_curses_window, tui_draw_vertical, tui_draw_horizontal, "%.3f:", action->index); //Print option index to 3 decimal places
						tui_draw_horizontal += 7; //Ready the cursor position for the options

						//Dirty macro that sets bitflags really nice. Basically prints enum names, and toggles them on and off based on key input and cursor position
#define BITFLAG_OPTION(NAME) \
						wattrset(tui_curses_window, A_NORMAL); \
						if (tui_sel_horizontal == horizontal_index && tui_sel_vertical == tui_draw_vertical) { \
							if (kbd_input_latest == '\n') (int&)action->action ^= NAME; \
							wattron(tui_curses_window, horizontal_index == tui_sel_horizontal ? A_UNDERLINE : !A_UNDERLINE); \
						} \
						wattron(tui_curses_window, action->action & NAME ? A_STANDOUT : !A_STANDOUT); \
						mvwprintw(tui_curses_window, tui_draw_vertical, tui_draw_horizontal, "%s", #NAME); \
						tui_draw_horizontal += strlen(#NAME) + 1; \
						horizontal_index++;

						BITFLAG_OPTION(Up);
						BITFLAG_OPTION(Down);
						BITFLAG_OPTION(Intake_In);
						BITFLAG_OPTION(Intake_Expel);
#undef BITFLAG_OPTION
					}
					++action;
				}
				tui_draw_vertical++;
			}

			char number_buffer[50];
#define FLOAT_OPTION(NAME) \
			if (kbd_input_latest == '\n' && tui_sel_vertical == tui_draw_vertical) { \
				mvwprintw(tui_curses_window, tui_draw_vertical, 4, "%s: ", #NAME); \
				wrefresh(tui_curses_window); \
				echo(); curs_set(1); \
				mvgetstr(tui_draw_vertical, strlen(#NAME) + 6, number_buffer); \
				noecho(); curs_set(0); \
				node.NAME = atof(number_buffer); \
			} \
			wattrset(tui_curses_window, tui_sel_vertical == tui_draw_vertical ? A_STANDOUT : !A_STANDOUT); \
			mvwprintw(tui_curses_window, tui_draw_vertical, 4, "%s: %.3f", #NAME, node.NAME); \
			tui_draw_vertical++;

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

void PathEditor::tui_edit_node(Node* node) {
	node->is_open ^= kbd_input_latest == '\n' && tui_sel_vertical == tui_draw_vertical; //Toggle being expanded with enter key
	wattrset(tui_curses_window, tui_sel_vertical == tui_draw_vertical ? A_STANDOUT : !A_STANDOUT); //If selected, highlight
	mvwprintw(tui_curses_window, tui_draw_vertical, 1, "[%s] %s", node->is_open ? "-" : "+", node->name); //Print the name, and [+] if closed and [-] if open
	tui_draw_vertical++; //Move the drawing cursor down
}

void PathEditor::tui_edit_action(TimedAction* action) {
}

void PathEditor::tui_edit_number(char* name, float& number) {
}

void PathEditor::tui_edit_bitflag(char* name, int& flag, int& flagedit) {
}

bool PathEditor::update_gui() {
	return true;
}

PathEditor::~PathEditor() {
	delwin(tui_curses_window);
	endwin();
}
