#pragma once
#ifndef __PIRACER_HPP__
# define __PIRACER_HPP__

#include <iostream>
#include <string>
#include <mutex>
#include <boost/python.hpp>

#define PIRACER_MODULE_NAME "vehicles"
#define PIRACER_CLASS_NAME "PiRacerStandard"

namespace py = boost::python;

class PiRacer
{
protected:
	PiRacer();
	~PiRacer();
public:
	PiRacer(PiRacer&) = delete;
	PiRacer& operator=(const PiRacer&) = delete;
	enum				MODE { MANUAL, AUTO };
	static PiRacer*		getInstance();
	bool				deleteInstance();
	const std::string&	getGear();
	const std::string&	getIndicator();
	MODE				getMode();
	bool				setGear(const std::string&);
	bool				setIndicator(const std::string&);
	bool				setThrottle(const double&);
	bool				setSteering(const double&);
	bool				setAutoMode();
	bool				setManualMode();
	bool				toggleMode();
	// void 				saveState(const std::string& filename) const; 
	// void 				restoreState(const std::string& filename);
private:
	std::string			_gear;
	std::string			_indicator;
	double				_steering;
	py::object			pModule;
	py::object			pClass;
	py::object			pInstance;
	static PiRacer*		_instance;
	static std::mutex	_mutex;
	double				_throttle;
	MODE				_mode;
};

#endif