#pragma once
#include <node.h>
#include <vector>
#include <curses.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

class NodeTui {
	public:
		NodeTui(std::vector<Node>* nodes) : nodes(nodes) {
			init();
		};
		bool update();
		~NodeTui();
	private:
		std::vector<Node>* nodes;
		WINDOW *curses_window;

		int sel_vertical; //Selection y position
		int sel_horizontal; //Selection x position

		int draw_vertical; //Cursor x position
		int draw_horizontal; //Cursor y positon
		int draw_idx_horizontal; //Drawing x position

		int kbd_input_latest;

		void init();
		
		void edit_node(Node* node);
		void edit_action(TimedAction* action);
		void edit_number(float& number);
		void edit_string(char* input);
		void string(char* input);
		void selectable_string(char* input);
		void edit_bitflag(char* name, int flag, int& flagedit);
		void next_line(int items, int indent);
		bool am_selected();
		bool pressed(int keycode);
};
