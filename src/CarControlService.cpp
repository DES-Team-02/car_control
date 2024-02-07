/*includes*/
#include <CommonAPI/CommonAPI.hpp>
#include "CarControlStubImpl.hpp"
#include "GamePad.hpp"
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

void schalter(std::string gear, PiRacer* piracer, std::shared_ptr<CarControlStubImpl> myservice)
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

void blinker(std::string indicator, PiRacer* piracer, std::shared_ptr<CarControlStubImpl> myservice)
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
	GamePad* gamepad = GamePad::getInstance();
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
	// set initial gear and indicator
	myservice->setGearAttribute("P");

    /*main loop */
    while (true)
	{
		// locks python interpreter
		PyGILState_STATE gilState = PyGILState_Ensure();
		// read input from gamepad
		Input input = gamepad->readInput();
		// handle input
		if (input.button_x)
			schalter("N",piracer,myService);
		else if (input.button_y)
			schalter("R",piracer,myService);
		else if (input.button_a)
			schalter("D",piracer,myService);
		else if (input.button_b)
			schalter("P",piracer,myService);
		if (input.button_l1)
			blinker("Left",piracer,myService);
		else if (input.button_r1)
			blinker("Right",piracer,myService);
		else
			// not nice but we need to see a toggle if the button is pushed multiple times
			blinker("None",piracer,myService); 
		// set attributes to piracer
		piracer->setThrottle(input.analog_stick_right.y * 0.5); // throttle reduced to 50%
		piracer->setSteering(input.analog_stick_left.x * (-1)); // steering inverted
		// release python interpreter
		PyGILState_Release(gilState);
		// save state
		//piracer->saveState("state.txt");
	}
    return 0;
}