/*includes*/
#include <CommonAPI/CommonAPI.hpp>
#include "CarControlStubImpl.hpp"
#include "CanReceiverProxy.hpp"
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

double setThrottleLimit(sonarArr_t _sonarArr)
{
	unsigned int min = std::min(
		{
			_sonarArr.getSensorfrontleft(),
			_sonarArr.getSensorfrontmiddle(),
			_sonarArr.getSensorfrontright()
		}
	);
	return std::min(0.5, min * 0.01 - 0.05);
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
	// set initial gear
	std::cout << "Set Initial Gear P to Attribute and Service." << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	changeGear("P",piracer,myService);

	domain      = "local";
    instance    = "commonapi.CanReceiver";
    connection  = "client-CanReceiver";
	std::shared_ptr<typename CommonAPI::DefaultAttributeProxyHelper<CanReceiverProxy, CommonAPI::Extensions::AttributeCacheExtension>::class_t> CanProxy;
	CanProxy = runtime->buildProxyWithDefaultAttributeExtension<CanReceiverProxy, CommonAPI::Extensions::AttributeCacheExtension>(domain, instance, connection);
	while (!CanProxy->isAvailable()) {
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
	std::cout << "CanReceiver service is available" << std::endl;

    /*main loop */
    while (true)
	{
		double throttle_limit = setThrottleLimit(CanProxy->getSonar());
		// locks python interpreter
		PyGILState_STATE gilState = PyGILState_Ensure();
		// read input from gamepad
		Input input = gamepad->readInput();
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
		else
			// not nice but we need to see a toggle if the button is pushed multiple times
			changeIndicator("None",piracer,myService); 
		// set attributes to piracer
		piracer->setThrottle(input.analog_stick_right.y * throttle_limit); // throttle reduced to 50%
		piracer->setSteering(input.analog_stick_left.x * (-1)); // steering inverted
		// release python interpreter
		PyGILState_Release(gilState);
		// save state
		//piracer->saveState("state.txt");
	}
    return 0;
}