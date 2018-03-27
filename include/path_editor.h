#pragma once
#include <node.h>
#include <vector>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

class PathEditor {
	public:
		PathEditor(std::vector<Node>* nodes) : nodes(nodes) {
			init();
		};
		bool update();
		~PathEditor();
	private:
		std::vector<Node>* nodes;
		WINDOW *tui_curses_window;

		int tui_sel_vertical; //Selection y position
		int tui_sel_horizontal; //Selection x position

		int tui_draw_vertical; //Cursor x position
		int tui_draw_horizontal; //Cursor y positon
		int tui_draw_idx_horizontal; //Drawing x position

		int kbd_input_latest;

		void init();
		void init_tui();
		void init_gui();
		
		bool update_tui();
		bool update_gui();

		void tui_edit_node(Node* node);
		void tui_edit_action(TimedAction* action);
		void tui_edit_number(char* name, float& number);
		void tui_edit_bitflag(char* name, int flag, int& flagedit);
		void tui_next_line(int items);
		bool tui_am_selected();
		bool tui_pressed(int keycode);
};
