#pragma once

#include "Joystick.hpp"

class Vector3f 
{
public:
	float x, y, z;

	Vector3f() : x(0.0f), y(0.0f), z(0.0f) {}
};

class ShanWanGamepadInput {
public:
	Vector3f analog_stick_left;
	Vector3f analog_stick_right;
	float button_l1, button_l2, button_r1, button_r2;
	bool button_x, button_a, button_b, button_y;
	bool button_select, button_start, button_home;

	ShanWanGamepadInput()
		: button_l1(0.0f), button_l2(0.0f), button_r1(0.0f), button_r2(0.0f),
		  button_x(false), button_a(false), button_b(false), button_y(false),
		  button_select(false), button_start(false), button_home(false) {}
};

class ShanWanGamepad : public Joystick {
public:
	ShanWanGamepadInput gamepad_input;

	ShanWanGamepad(const std::string& dev_fn = "/dev/input/js0");
	ShanWanGamepadInput read_data();
};