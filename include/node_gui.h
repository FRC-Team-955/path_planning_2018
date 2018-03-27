#pragma once

#include <node.h>
#include <vector>

#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <opencv2/opencv.hpp>

class NodeGui {
	public:
		NodeGui(std::vector<Node>* nodes) : nodes(nodes) {
			init();
		};
		void update();
	private:
		void init();
};
