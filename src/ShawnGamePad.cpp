#include "ShawnGamePad.hpp"

ShanWanGamepad::ShanWanGamepad(const std::string& dev_fn = "/dev/input/js0")
: Joystick(dev_fn) 
{
	extern errno;

	if (!Joystick::init())
		std::cerr << errno << std::endl;
}

ShanWanGamepadInput ShanWanGamepad::read_data()
{
	auto [button_name, button_number, button_state, axis_name, axis_number, axis_val] = Joystick::poll();

	// Joysticks
	if (axis_number == 0) {
		gamepad_input.analog_stick_left.x = axis_val;
	} else if (axis_number == 1) {
		gamepad_input.analog_stick_left.y = -axis_val;
	} else if (axis_number == 3) {
		gamepad_input.analog_stick_right.x = axis_val;
	} else if (axis_number == 4) {
		gamepad_input.analog_stick_right.y = -axis_val;
	}
	// Buttons
	else if (button_number == 0) {
		gamepad_input.button_a = button_state;
	} else if (button_number == 1) {
		gamepad_input.button_b = button_state;
	} else if (button_number == 2) {
		gamepad_input.button_x = button_state;
	} else if (button_number == 3) {
		gamepad_input.button_y = button_state;
	} else if (button_number == 4) {
		gamepad_input.button_l1 = button_state;
	} else if (button_number == 5) {
		gamepad_input.button_r1 = button_state;
	} else if (button_number == 6) {
		gamepad_input.button_select = button_state;
	} else if (button_number == 7) {
		gamepad_input.button_start = button_state;
	} else if (button_number == 8) {
		gamepad_input.button_home = button_state;
	}

	return gamepad_input;
}