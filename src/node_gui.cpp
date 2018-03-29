#include <node_gui.h>

void reshape(int w, int h) {
	int min = std::min(w, h);
	glViewport(0, 0, (GLsizei)min, (GLsizei)min);
}

void fake_display() {}

cv::Point2f mouse(0.0, 0.0);
bool mouse_active_b = false;

void mouse_adjust(int mx, int my) {
	/*
		my = glutGet(GLUT_WINDOW_HEIGHT) - my;
		mouse.y =
		(((double)my / (double)glutGet(GLUT_WINDOW_HEIGHT)) * FD::field_bounds.height) -
		(FD::field_bounds.height / 2.0); mouse.x =
		(((double)mx / (double)glutGet(GLUT_WINDOW_HEIGHT)) * FD::field_bounds.width) -
		(FD::field_bounds.width / 2.0);
		*/
	GLint viewport[4];
	GLdouble modelview[16], projection[16];
	GLfloat wx = mx, wy, wz;

	glGetIntegerv(GL_VIEWPORT, viewport);
	mx = viewport[3] - my;
	wy = viewport[2] - my;
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glReadPixels(mx, my, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &wz);
	GLdouble ox = 0.0, oy = 0.0, oz = 0.0;
	gluUnProject(wx, wy, wz, modelview, projection, viewport, &ox, &oy, &oz);
	mouse = cv::Point2f(ox, oy);
}

void mouse_passive(int mx, int my) {
	mouse_adjust(mx, my);
	mouse_active_b = false;
}

void mouse_active(int mx, int my) {
	mouse_adjust(mx, my);
	mouse_active_b = true;
}

void NodeGui::init() {
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int argc = 0;
	glutInit(&argc, nullptr);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(1400, 1400);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Node Editor");
	glutReshapeFunc(reshape);
	glutDisplayFunc(fake_display);
	bound(FD::field_bounds, 1000.0);
	glutMotionFunc(mouse_active);
	glutPassiveMotionFunc(mouse_passive);
	glLineWidth(2);
}

//TODO: Split this into smaller functions
void NodeGui::update(std::vector<Node>& nodes, TankDrive::TankOutput robot) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	bound(FD::field_bounds, 1000.0);
	grid(1000.0, 1000.0, 0.2, 0.2, 0.2, FD::field_bounds);
	render_field();

	float smalldist = 999999.0;
	Node *tempnode = nullptr;
	if (nodes.size() > 0) {
		auto node = nodes.begin();
		do {
			glColor3f(1.0, 1.0, 1.0);

			glBegin(GL_POLYGON);
			const float sidelen = 25.0;
			glVertex2f(node->position.x + sidelen, node->position.y + sidelen);
			glVertex2f(node->position.x + sidelen, node->position.y - sidelen);
			glVertex2f(node->position.x - sidelen, node->position.y - sidelen);
			glVertex2f(node->position.x - sidelen, node->position.y + sidelen);
			glEnd();


			//TODO: Remove this macro, eww
#define DRAW_SEGMENT(NAME)                                                         \
			cv::Point2f NAME = node->get_##NAME##_ctrlpt();                              \
			glBegin(GL_LINES);                                                           \
			glVertex2f(node->position.x, node->position.y);                              \
			glVertex2f(NAME.x, NAME.y);                                                  \
			glEnd();

			if (node->is_open) {
				glColor3f(1.0, 0.1, 0.1);
				DRAW_SEGMENT(in);
				glColor3f(0.1, 1.0, 0.1);
				DRAW_SEGMENT(out);
				float dist = node->get_distance_to_closest_component(mouse);
				if (dist < smalldist) {
					smalldist = dist;
					tempnode = &*node;
				}
			}
#undef DRAW_SEGMENT

			glColor3f(1.0, 1.0, 1.0);
			draw_string(node->position, node->name);
		} while (++node != nodes.end()); 

		TankDrive::Traversal trav (nodes.begin(), nodes.end(), 635.0);
		glBegin(GL_LINES);
		TankDrive::TankOutput output;
		Action act;
		cv::Point2f last_left = node->position;
		cv::Point2f last_center = node->position;
		cv::Point2f last_right = node->position;
		unsigned int iters = 0;
		while (iters < 5000 && trav.next(output, act, 10.0)) {
			if (iters > 1 && iters != 5000) {
				color_by(output.motion.velocity_left);
				glVertex2f(output.left_position.x, output.left_position.y);
				glVertex2f(last_left.x, last_left.y);

				color_by(output.motion.velocity_right);
				glVertex2f(output.right_position.x, output.right_position.y);
				glVertex2f(last_right.x, last_right.y);

				glColor3f(act & Action::Cube_Expel, act & Action::Cube_Intake, (act & Action::Scissor_Scale) + (act & Action::Scissor_Switch));
				glVertex2f(output.center_position.x, output.center_position.y);
				glVertex2f(last_center.x, last_center.y);
			}

			last_left = output.left_position;
			last_right = output.right_position;
			last_center = output.center_position;
			iters++;
		}
		glEnd();
	}

	glColor3f(0.8, 0.8, 0.8);
	glLineWidth(3);
	std::vector<cv::Point2f> wireframe;
	wireframe.push_back(cv::Point2f ((635.0), (635.0)));
	wireframe.push_back(cv::Point2f (-(635.0), (635.0)));
	wireframe.push_back(cv::Point2f (-(635.0), -(635.0)));
	wireframe.push_back(cv::Point2f ((635.0), -(635.0)));

	cv::Mat rot_mat( 2, 3, CV_32FC1 );
	rot_mat = cv::getRotationMatrix2D(cv::Point2f(0.0, 0.0), -robot.robot_direction * (180.0 / acos(-1)), 1.0);
	cv::transform(wireframe, wireframe, rot_mat);

	cv::Point2f last = wireframe.back();
	glBegin(GL_LINES);
	for (auto& point : wireframe) {
		glVertex2f(point.x + robot.center_position.x, point.y + robot.center_position.y);
		glVertex2f(last.x + robot.center_position.x, last.y + robot.center_position.y);
		last = point;
	}
	glVertex2f(robot.center_position.x, robot.center_position.y);
	glVertex2f(robot.center_position.x + (cos(robot.robot_direction) * (635.0)), robot.center_position.y + (sin(robot.robot_direction) * (635.0)));
	glEnd();

	if (mouse_active_b) {
		if (movednode) {
			movednode->set_closest_component(mouse);
		} else if (tempnode) {
			movednode = tempnode;
		} else {
			movednode = nullptr;
		}
	} else {
		movednode = nullptr;
	}

	glutSwapBuffers();
	glutPostRedisplay();
	glutMainLoopEvent();
}

void NodeGui::render_field() {
	// Field bounds
	color_by_3f(we_are_blue ? blue : red);
	glBegin(GL_LINES);
	glVertex2f(0.0, FD::side_station_h);
	glVertex2f(FD::side_station_w, 0.0);
	glVertex2f(FD::field_back_w, 0.0);
	glVertex2f(FD::field_back_w + FD::side_station_w, FD::side_station_h);
	glEnd();

	render_rect(FD::field_bounds);

#define RENDER_SWSC(NAME, NUM)                                                  \
	color_by_3f(grey);                                                           \
	render_rect(FD::NAME::boom);                                                 \
	\
	color_by_3f((we_are_blue == (config[NUM] == 'L')) ? blue : red);             \
	render_rect(FD::NAME::left_plate);                                           \
	\
	color_by_3f((we_are_blue == (config[NUM] == 'R')) ? blue : red);             \
	render_rect(FD::NAME::right_plate);

	RENDER_SWSC(Switch, 0);
	RENDER_SWSC(Scale, 1);

#undef RENDER_SWSC
}

void NodeGui::render_rect(cv::Rect2f input) {
	glLineWidth(2);
	glBegin(GL_LINES);

	glVertex2f(input.tl().x, input.tl().y);
	glVertex2f(input.br().x, input.tl().y);

	glVertex2f(input.br().x, input.tl().y);
	glVertex2f(input.br().x, input.br().y);

	glVertex2f(input.br().x, input.br().y);
	glVertex2f(input.tl().x, input.br().y);

	glVertex2f(input.tl().x, input.br().y);
	glVertex2f(input.tl().x, input.tl().y);
	glEnd();
}

void NodeGui::color_by_3f(cv::Point3f rgb) { glColor3f(rgb.x, rgb.y, rgb.z); }

void NodeGui::grid(float step_x, float step_y, float r, float g, float b,
		cv::Rect2f screen_dim) {
	screen_dim.x -= step_x;
	screen_dim.y -= step_y;
	screen_dim.width += step_x;
	screen_dim.height += step_y;
	glLineWidth(1);
	glColor3f(r, g, b);
	for (float i = step_y * (int)(screen_dim.y / step_y); i < screen_dim.br().y;
			i += step_y) {
		if (i == 0.0) {
			glColor3f(r + 0.2, g + 0.2, b + 0.2);
		} else {
			glColor3f(r, g, b);
		}
		glBegin(GL_LINES);
		glVertex2f(screen_dim.x, i);
		glVertex2f(screen_dim.br().x, i);
		glEnd();
	}
	for (float i = step_x * (int)(screen_dim.x / step_x); i < screen_dim.br().x;
			i += step_x) {
		if (i == 0.0) {
			glColor3f(r + 0.2, g + 0.2, b + 0.2);
		} else {
			glColor3f(r, g, b);
		}
		glBegin(GL_LINES);
		glVertex2f(i, screen_dim.y);
		glVertex2f(i, screen_dim.br().y);
		glEnd();
	}
}

void NodeGui::bound(cv::Rect2f input, float max_z) {
	glLoadIdentity();
	float min = std::min(input.tl().x, input.tl().y);
	float max = std::max(input.br().x, input.br().y);
	glOrtho(min, max, min, max, max_z, -max_z);
}

void NodeGui::draw_string(cv::Point2f pos, char *input) {
	glRasterPos2f(pos.x, pos.y);
	size_t len = (int)strlen(input);
	for (size_t i = 0; i < len; i++)
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, input[i]);
}

void NodeGui::color_by(float input) { //Green to black to red from 1.0 to 0.0 to -1.0 respectively
	if (input > 0) {
		glColor3f(0.0, input, 0.0);
	} else {
		glColor3f(fabs(input), 0.0, 0.0);
	}
}
