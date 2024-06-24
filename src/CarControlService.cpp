/*includes*/
#include <CommonAPI/CommonAPI.hpp>
#include "CarControlStubImpl.hpp"
#include "CanProxy.hpp"
#include "GamePad.hpp"
#include "ShawnGamePad.hpp"
#include "PiRacer.hpp"
#include <iostream>
#include <chrono>
#include <csignal>
#include <cstdlib>
#include <vector>
#include <unistd.h>
#include <Python.h>
#include <thread>
#include <fstream>
#include "JetsonProxyImpl.hpp"
#include "JoyStick.hpp"
#include "Joystick.hpp"

/* signal handler */
void signalHandler(int signum)
{
	std::cout << "\n signal: " << signum << std::endl;
	std::cout << "Shutting down..." << std::endl;
	exit(128 + signum);
}

/* clean up */
void cleanUp()
{
    // delete singeltons
	GamePad* gamepad = GamePad::getInstance();
	PiRacer* piracer = PiRacer::getInstance();
	piracer->deleteInstance();
	gamepad->deleteInstance();
    // finalize python interpreter
	Py_Finalize();
	std::cout << "cleanUp(): Python Interpreter Finalized." << std::endl;
}

void changeGear(std::string gear, PiRacer* piracer, std::shared_ptr<CarControlStubImpl> myservice)
{
	std::cout << " ---------------------------" << std::endl;
	std::cout << "GamePad tries to set gear. " << std::endl;
	std::cout << "  Input Gear: " << gear << std::endl;
	// set gear to piracer
	bool accepted = piracer->setGear(gear);	
	std::cout << (accepted ? "->Gear change accepted." : "->Gear change declined.") << std::endl;
	// if gear is set by gamepad, set gear attribute to service
	if(accepted){
		myservice->setGearAttribute(gear);
	} 
	std::cout << " ---------------------------" << std::endl;
}

void changeIndicator(std::string indicator, PiRacer* piracer, std::shared_ptr<CarControlStubImpl> myservice)
{
	piracer->setIndicator(indicator);
	myservice->setIndicatorAttribute(indicator);
}

/*main*/
int main() { 
    /*runtime setups*/
    // signal handler
    signal(SIGINT, signalHandler);
	signal(SIGQUIT, signalHandler);
    // python interpreter
	setenv("PYTHONPATH", "../piracer", 0);
	Py_Initialize();
	atexit(cleanUp);

    /*create singeltons gamepad and piracer */
	// GamePad* gamepad = GamePad::getInstance();
	ShanWanGamepad gamepad("/dev/input/js0");
	PiRacer* piracer = PiRacer::getInstance();

    /*setup CommonAPI service*/
    // get runtime
    std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();
    // define service's domain, instance, connection
    std::string domain      = "local";
    std::string instance    = "commonapi.CarControl";
    std::string connection  = "service-CarControl";
    // create service
    std::shared_ptr<CarControlStubImpl> myService = std::make_shared<CarControlStubImpl>();
    // register service
    bool successfullyRegistered = runtime->registerService(domain, instance, myService, connection);
    // if registration failed, try again
    while (!successfullyRegistered) {
        std::cout << "Register Service failed, trying again in 100 milliseconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        successfullyRegistered = runtime->registerService(domain, instance, myService, connection);
    }
    std::cout << "Successfully Registered Service!" << std::endl;
	// set initial gear
	std::cout << "Set Initial Gear P to Attribute and Service." << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	changeGear("P",piracer,myService);

	std::shared_ptr<JetsonProxyImpl> jetsonService = std::make_shared<JetsonProxyImpl>();
	jetsonService->subscribeSteering();
	jetsonService->subscribeThrottle();

	std::shared_ptr<CanProxy> canService = std::make_shared<CanProxy>();
	canService->subscribe_sonar();

	double steering = 0;
	double throttle = 0;
    /*main loop */
    while (true)
	{
		// locks python interpreter
		// read input from gamepad
		// Input input = gamepad->readInput();
		ShanWanGamepadInput input = gamepad.read_data();
		// handle input
		if (input.button_x)
			changeGear("N",piracer,myService);
		else if (input.button_y)
			changeGear("R",piracer,myService);
		else if (input.button_a)
			changeGear("D",piracer,myService);
		else if (input.button_b)
			changeGear("P",piracer,myService);
		if (input.button_l1)
			changeIndicator("Left",piracer,myService);
		else if (input.button_r1)
			changeIndicator("Right",piracer,myService);
		else if (input.button_select)
			piracer->setManualMode();
		else if (input.button_start)
			piracer->setAutoMode();
		else
			// not nice but we need to see a toggle if the button is pushed multiple times
			changeIndicator("None",piracer,myService);
		if (piracer->getMode() == PiRacer::MODE::MANUAL)
		{
			steering = input.analog_stick_left.x * (-1); // steering inverted
			throttle = input.analog_stick_right.y * (0.35); // throttle reduced to 50%
		}
		else if (piracer->getMode() == PiRacer::MODE::AUTO)
		{
			steering = jetsonService->getSteering();
			throttle = jetsonService->getThrottle();
		}

		Sonar_t sonar = canService->getSonar();
		unsigned int sonarValue = piracer->getGear() == "D" 
								? sonar.getSonarFront()
								: sonar.getSonarRear();

		if (sonarValue < 50 && sonarValue >= 40)
			throttle *= 0.75;
		else if (sonarValue < 40 && sonarValue >= 30)
			throttle *= 0.5;
		else if (sonarValue < 30 && sonarValue >= 20)
			throttle *= 0.25;
		else if (sonarValue < 20 && sonarValue >= 5)
			throttle *= 0.15;
		else if (sonarValue < 5)
			throttle *= 0;

		// set attributes to piracer
		PyGILState_STATE gilState = PyGILState_Ensure();
		piracer->setSteering(steering);
		piracer->setThrottle(throttle);
		// release python interpreter
		PyGILState_Release(gilState);
		// save state
		//piracer->saveState("state.txt");
	}
    return 0;
}