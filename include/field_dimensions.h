#ifndef FIELD_DIMENSIONS_H
#define FIELD_DIMENSIONS_H

#include <opencv2/opencv.hpp>
#define IN_TO_MM_CONVERSION_FACTOR 25.4
#define IN_TO_MM(in) (in * IN_TO_MM_CONVERSION_FACTOR)

#define MM_TO_IN(mm) (mm / IN_TO_MM_CONVERSION_FACTOR)

namespace FieldDimension {
	const float field_w = IN_TO_MM(314.0);
	const float field_h = IN_TO_MM(400.0);

	const float side_station_w = IN_TO_MM(48.0);
	const float side_station_h = IN_TO_MM(36.0);
	const float field_back_w = IN_TO_MM(264.0);

	const cv::Rect2f field_bounds (0, 0, field_w, field_h);	

	const float plate_w = IN_TO_MM(36.0);
	const float plate_h = IN_TO_MM(48.0);

#define CREATE_SWSC \
	const cv::Rect2f left_plate ( \
			plate_x, \
			plate_y, \
			plate_w, \
			plate_h); \
	\
	const cv::Rect2f right_plate ( \
			plate_x + boom_w + plate_w, \
			plate_y, \
			plate_w, \
			plate_h); \
	\
	const cv::Rect2f boom ( \
			plate_x + plate_w, \
			plate_y + (plate_h / 2.0) - (boom_h / 2.0), \
			boom_w, \
			boom_h); \
	\
	const cv::Point2f front_left ((left_plate.br().x + left_plate.tl().x) / 2.0, left_plate.tl().y); \
	const cv::Point2f back_left ((left_plate.br().x + left_plate.tl().x) / 2.0, left_plate.br().y); \
	const cv::Point2f front_right ((right_plate.br().x + right_plate.tl().x) / 2.0, right_plate.tl().y); \
	const cv::Point2f back_right ((right_plate.br().x + right_plate.tl().x) / 2.0, right_plate.br().y);

	namespace Switch {
		const float boom_w = IN_TO_MM(72.0);
		const float boom_h = IN_TO_MM(12.5);
		const float plate_y = IN_TO_MM(140.0);
		const float plate_x = IN_TO_MM(85.25);
		CREATE_SWSC
	}

	namespace Scale {
		const float boom_w = IN_TO_MM(98.0); //TODO: Double check this
		const float boom_h = IN_TO_MM(12.5);
		const float plate_y = IN_TO_MM(299.65);
		const float plate_x = IN_TO_MM(71.57);
		CREATE_SWSC
	}
}
#undef CREATE_SWSC

#undef IN_TO_MM_CONVERSION_FACTOR
#undef IN_TO_MM
#undef MM_TO_IN

#endif
