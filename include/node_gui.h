#pragma once

#include <node.h>
#include <vector>

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#undef OK
#include <opencv2/opencv.hpp>
#include <field_dimensions.h>

namespace FD = FieldDimension;
class NodeGui {
	public:
		NodeGui(std::vector<Node>* nodes) : nodes(nodes) {
			init();
			we_are_blue = true;
			config[0] = 'R';
			config[1] = 'L';
			config[2] = 'L';
		};
		void update();
	private:
		cv::Point3f white = cv::Point3f (1.0, 1.0, 1.0);
		cv::Point3f grey = cv::Point3f (0.8, 0.8, 0.8);
		cv::Point3f red = cv::Point3f (1.0, 0.3, 0.3);
		cv::Point3f blue = cv::Point3f (0.2, 0.5, 1.0);

		std::vector<Node>* nodes;
		void init();
		void render_rect(cv::Rect2f input);
		void color_by_3f(cv::Point3f rgb);
		void grid(float step_x, float step_y, float r, float g, float b, cv::Rect2f screen_dim);
		void bound(cv::Rect2f input, float max_z);
		void render_field();
		void draw_string(cv::Point2f pos, char *input);
		bool we_are_blue;
		char config[3];
		Node* movednode;
};
