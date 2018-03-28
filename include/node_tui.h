#pragma once
#include <node.h>
#include <vector>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

class NodeTui {
	public:
		NodeTui() {
			init();
		};
		bool update(std::vector<Node>& nodes);
		~NodeTui();
	private:
		WINDOW *curses_window;

		int sel_vertical = 0; //Selection y position
		int sel_horizontal = 0; //Selection x position

		int draw_vertical = 0; //Cursor x position
		int draw_horizontal = 0; //Cursor y positon
		int draw_idx_horizontal = 0; //Drawing x position

		int kbd_input_latest;

		void init();
		
		void edit_node(Node* node);
		void edit_action(TimedAction* action);
		void edit_number(float& number);
		void edit_string(char* input);
		void edit_bool(bool& input, char* if_true, char* if_false);
		void string(char* input);
		void selectable_string(char* input);
		void edit_bitflag(char* name, int flag, int& flagedit);
		void next_line(int items, int indent);
		bool am_selected();
		bool pressed(int keycode);
};
