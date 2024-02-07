#include "PiRacer.hpp"

//Initialize static memeber variables
PiRacer* PiRacer::_instance{nullptr};
std::mutex PiRacer::_mutex;

PiRacer::PiRacer():
	_gear("P"),
	_indicator("None"),
	_throttle(0.0),
	_steering(0.0)
{
	try
	{
		pModule = py::import(PIRACER_MODULE_NAME);
		pClass = pModule.attr(PIRACER_CLASS_NAME);
		pInstance = pClass();
	}
	catch(const py::error_already_set&)
	{
		PyErr_Print();
		exit(1);
	}
	std::cout << "PiRacer instance created." << std::endl;
}

PiRacer::~PiRacer()
{
	std::cout << "PiRacer DestRemoving PiRacer instance." << std::endl;
}

PiRacer* PiRacer::getInstance()
{
	std::lock_guard<std::mutex> lock(_mutex);
	if(_instance == nullptr)
	{
		std::cout << "Creating PiRacer instance." << std::endl;
		_instance = new PiRacer();
	}
	return _instance;
}

bool PiRacer::deleteInstance()
{	
	std::cout << "PiRacer's deleteInstance() called" << std::endl;
	std::lock_guard<std::mutex> lock(_mutex);
	if(_instance != nullptr)
	{
		std::cout << _instance << std::endl;
		std::cout << "Deleting PiRacer instance." << std::endl;
		delete _instance;
		_instance = nullptr;
		return true;
	}
    return false;
}

const std::string& PiRacer::getGear()
{
	return _gear;
}

const std::string& PiRacer::getIndicator()
{
	return _indicator;
}

bool PiRacer::setGear(const std::string& gear)
{
	//std::cout << "Abs throttel: " << abs(_throttle) << std::endl;
	if (abs(_throttle) >= 0.01)
		return false;
	_gear = gear;
	//std::cout << "PiRacer - Gear set to: " << _gear << std::endl;
	py::object setThrottle = pInstance.attr("set_throttle_percent");
	setThrottle(0.0);
	return true;
}

bool PiRacer::setIndicator(const std::string& indicator)
{
	if (_indicator == indicator)
		return false;
	_indicator = indicator;
	//std::cout << "PiRacer - Indicator set to: " << indicator << std::endl;
	return true;
}

bool PiRacer::setThrottle(const double& throttle)
{
	if ((_gear == "P" || _gear == "N"))
		return false;
	if (_gear == "D" && throttle < -0.1)
		return false;
	if (_gear == "R" && throttle > 0)
		return false;
	_throttle = throttle;
	//std::cout << "throttle set to: " << throttle * 100  << "%" << std::endl;
	py::object pysetThrottle = pInstance.attr("set_throttle_percent");
	pysetThrottle(_throttle);
	return true;
}

bool PiRacer::setSteering(const double& steering)
{
	_steering = steering;
	//std::cout << "steering set to: " << steering << std::endl;
	py::object pysetSteering = pInstance.attr("set_steering_percent");
	pysetSteering(_steering);
	return true;
}

// void PiRacer::saveState(const std::string &filename) const
// {
// 	// save attributes to file
// 	std::ofstream file(filename);
// 	if (file.is_open()) {
// 		file << "gear: " 		<< _gear 		<< std::endl;
// 		file << "indicator: " 	<< _indicator 	<< std::endl;
// 		file << "throttle: " 	<< _throttle 	<< std::endl;
// 		file << "steering: " 	<< _steering 	<< std::endl;
// 		file.close();
// 	}
// 	else {
// 		std::cerr << "Error opening file for saving state." << std::endl;
// 	}
// }

// void PiRacer::restoreState(const std::string &filename)
// {
// 	// restore attributes from file
// 	std::ifstream file(filename);
// 	if (file.is_open()) {
// 		std::string line;
// 		while (std::getline(file, line)) {
// 			std::istringstream is_line(line);
// 			std::string key;
// 			if (std::getline(is_line, key, ':')) {
// 				std::string value;
// 				if (std::getline(is_line, value)) {
// 					if (key == "gear") {
// 						_gear = value;
// 					}
// 					else if (key == "indicator") {
// 						_indicator = value;
// 					}
// 					else if (key == "throttle") {
// 						_throttle = std::stod(value);
// 					}
// 					else if (key == "steering") {
// 						_steering = std::stod(value);
// 					}
// 				}
// 			}
// 		}
// 		file.close();
// 	}
// 	else {
// 		std::cerr << "Error opening file for restoring state." << std::endl;
// 	}
// }
