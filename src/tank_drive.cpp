#include <tank_drive.h>

float TankDrive::evaluate(ParametricOutput parametric,
		TankDrive::TankOutput &output, float max_velocity,
		float dt, float wheel_distance) {
	// Position = center + (perpendicular vector * d)
	output.center_position = parametric.position;
	output.left_position =
		parametric.position +
		(wheel_distance * parametric.perpendicular_unit_vector_xy());
	output.right_position =
		parametric.position -
		(wheel_distance * parametric.perpendicular_unit_vector_xy());

	// How far in position we expect to go over dj
	cv::Point2f chage_pos_left =
		(parametric.velocity) +
		(wheel_distance * parametric.perpendicular_unit_vector_derivative_xy());
	cv::Point2f chage_pos_right =
		(parametric.velocity) -
		(wheel_distance * parametric.perpendicular_unit_vector_derivative_xy());

	// Magnitude of change in position of the above over dj
	float dp_dj_left = cv::norm(chage_pos_left);
	float dp_dj_right = cv::norm(chage_pos_right);

	// Find dj for the time step
	float largest_dp_dj = std::max(dp_dj_left, dp_dj_right);
	float max_dp = max_velocity * dt;
	float dj = (max_dp / largest_dp_dj);

	// Distances travelled per dt
	float dp_left = (dj * dp_dj_left);
	float dp_right = (dj * dp_dj_right);

	// Assign velocities
	output.motion.velocity_left = dp_left / dt;
	output.motion.velocity_right = dp_right / dt;

	// Increment positions
	output.motion.position_left += dp_left;
	output.motion.position_right += dp_right;

	// Robot heading
	output.robot_direction = parametric.direction_xy();

	// We are turning too hard, one side will have to reverse
	// Determined by the normal of each wheel's trajectory not being within 90
	// degrees of the trajectory of the robot center (It's reversing)
	float direction_left = atan2(chage_pos_left.y, chage_pos_left.x);
	float direction_right = atan2(chage_pos_right.y, chage_pos_right.x);
	if (fabs(direction_left - output.robot_direction) > M_PI / 2.0)
		output.motion.velocity_left *= -1.0;

	if (fabs(direction_right - output.robot_direction) > M_PI / 2.0)
		output.motion.velocity_right *= -1.0;

	// How far along the spline we advanced
	return dj;
}

bool TankDrive::Traversal::next(TankDrive::TankOutput &output, Action& action_out, float dt) {
	if (next_node != end_node && current_node != end_node) {
		time_this_node_start += dt;
		time_s += dt;
		if (actions != current_node->actions.end()) {
			action_out = actions->action;
			if (time_this_node_start >= actions->time)
				actions++;
		} else {
			action_out = Action::None;
		}
		index += TankDrive::evaluate(current_node->spline(next_node, index), output,
				current_node->speed_ramp(next_node, index) *
				(current_node->reverse ? -1.0 : 1.0),
				dt, wheel_distance);
		if ((index > 1.0 && !current_node->reverse) ||
				(index < 0.0 && current_node->reverse)) {
			if (next_node != end_node) {
				time_this_node_start = 0.0;
				current_node = next_node;
				next_node++;
				index = (current_node)->reverse ? 1.0 : 0.0;
				std::sort(current_node->actions.begin(), current_node->actions.end());
				actions = current_node->actions.begin();
			} else {
				return false;
			}
		}
		return true;
	} else {
		return false;
	}
}

void TankDrive::Traversal::reset() {
	if (current_node != end_node) {
		current_node = begin_node;
		next_node = begin_node + 1;
		index = (current_node)->reverse ? 1.0 : 0.0;
		actions = current_node->actions.begin();
		time_this_node_start = 0.0;
		time_s = 0.0;
	}
}
