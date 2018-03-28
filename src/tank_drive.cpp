#include <tank_drive.h>

float TankDrive::evaluate (ParametricOutput parametric, TankDrive::TankOutput& output, float max_velocity, float dt, float wheel_distance) {
	//Position = center + (perpendicular vector * d)
	output.center_position = parametric.position;
	output.left_position = parametric.position + (wheel_distance * parametric.perpendicular_unit_vector_xy());
	output.right_position = parametric.position - (wheel_distance * parametric.perpendicular_unit_vector_xy());

	//Magnitude of change in position of the above over dj
	float dp_dj_left = cv::norm((parametric.velocity) + (wheel_distance * parametric.perpendicular_unit_vector_derivative_xy()));
	float dp_dj_right = cv::norm((parametric.velocity) - (wheel_distance * parametric.perpendicular_unit_vector_derivative_xy()));

	//Find dj for the time step
	float largest_dp_dj = std::max(dp_dj_left, dp_dj_right);
	float max_dp = max_velocity * dt;
	float dj = (max_dp / largest_dp_dj);

	//Distances over dt as velocity
	float dp_left = (dj * dp_dj_left);
	float dp_right = (dj * dp_dj_right);

	//Assign velocities
	output.motion.velocity_left = dp_left / dt;
	output.motion.velocity_right = dp_right / dt;

	//Increment positions
	output.motion.position_left += dp_left;
	output.motion.position_right += dp_right;

	//TODO: Make the actual function exposed instead of this garbage?
	output.robot_direction = parametric.direction_xy();

	//Hard turns (Where one motor's velocity more than doubles the other's velocity over the wheel distance), reverse one motor
	if (((dp_dj_left - parametric.velocity_magnitude_xy()) / wheel_distance > 1.0) &&
			parametric. change_in_angle() > 0.0)
		output.motion.velocity_left *= -1.0;

	if (((dp_dj_right - parametric.velocity_magnitude_xy()) / wheel_distance > 1.0) &&
			parametric. change_in_angle() < 0.0)
		output.motion.velocity_right *= -1.0;

	//Increase the index by the amount we need to move one time unit
	return dj;
}
