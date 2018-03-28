#pragma once

#include <node.h>
#include <vector>

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#undef OK
#include <field_dimensions.h>
#include <opencv2/opencv.hpp>

#include <parametric_output.h>
#include <spline.h>
#include <tank_drive.h>

namespace FD = FieldDimension;
class NodeGui {
	public:
		NodeGui(float wheel_distance, char config[3], bool we_are_blue)
			: wheel_distance(wheel_distance),
			we_are_blue(we_are_blue) {
				strcpy(this->config, config);
				init();
			};
		void update(std::vector<Node> &nodes);

	private:
		cv::Point3f white = cv::Point3f(1.0, 1.0, 1.0);
		cv::Point3f grey = cv::Point3f(0.8, 0.8, 0.8);
		cv::Point3f red = cv::Point3f(1.0, 0.3, 0.3);
		cv::Point3f blue = cv::Point3f(0.2, 0.5, 1.0);

		void init();
		void render_rect(cv::Rect2f input);
		void color_by_3f(cv::Point3f rgb);
		void grid(float step_x, float step_y, float r, float g, float b,
				cv::Rect2f screen_dim);
		void bound(cv::Rect2f input, float max_z);
		void render_field();
		void draw_string(cv::Point2f pos, char *input);
		void color_by(float input);
		bool we_are_blue;
		char config[3];
		Node *movednode;
		float wheel_distance;
};
