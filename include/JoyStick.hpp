#pragma once

#include <fcntl.h>
#include <linux/joystick.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <sys/epoll.h>

class Joystick 
{
public:
	std::unordered_map<std::string, float> axis_states;
	std::unordered_map<std::string, int> button_states;
	std::unordered_map<int, std::string> axis_names;
	std::unordered_map<int, std::string> button_names;
	std::vector<std::string> axis_map;
	std::vector<std::string> button_map;
	int jsdev;
	int epoll_fd;
	std::string dev_fn;
	std::string js_name;
	int num_axes;
	int num_buttons;

	JoyStick();
	~Joystick();

	bool init();
	void show_map();
	std::tuple<std::string, int, int, std::string, int, float>poll();
};