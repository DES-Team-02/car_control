#include <iostream>
#include <string>
#include <unistd.h>
#include <cstdlib>
#include <CommonAPI/CommonAPI.hpp>
#include "../src-gen/core/v0/commonapi/CarControlProxy.hpp"
#include <thread>

using namespace v0::commonapi;

/* Callback for recieved messages using async calls*/
void recv_cb(const CommonAPI::CallStatus& callStatus, const bool& gearSelectionHeadUnit) {
    std::cout << " ---------------------------" << std::endl;
    std::cout << "Result of asynchronous call: " << std::endl;
    std::cout << "   callStatus: " << ((callStatus == CommonAPI::CallStatus::SUCCESS) ? "SUCCESS" : "NO_SUCCESS")<< std::endl;
    std::cout << "   Output : " << gearSelectionHeadUnit << std::endl;
    std::cout << " ---------------------------" << std::endl;
}

int main()
{
    /* Create Proxy */
    std::shared_ptr < CommonAPI::Runtime > runtime = CommonAPI::Runtime::get();
    std::string domain      = "local";
    std::string instance    = "commonapi.CarControl";
    std::string connection  = "service-CarControl";
    auto myProxy = runtime->buildProxy<CarControlProxy>(domain, instance, connection);
    std::cout << "Waiting for service to become available." << std::endl;
    while (!myProxy->isAvailable()){
        std::this_thread::sleep_for(std::chrono::microseconds(10));
    }

    /* Subscribe to Service Notifyer */
    CommonAPI::CallStatus callStatus;
    CommonAPI::CallInfo info(1000);
    info.sender_ = 1234;
    // Gear Subscription
    myProxy->getGearAttribute().getChangedEvent().subscribe([&](const std::string& gear) {
        std::cout << " ---------------------------" << std::endl;
        std::cout << "Service Notification: 'Gear' changed to : " << gear << std::endl;
        std::cout << " ---------------------------" << std::endl;
    });
    // Indicator Subscription
    myProxy->getIndicatorAttribute().getChangedEvent().subscribe([&](const std::string& indicator) {
        std::cout << " ---------------------------" << std::endl;
        std::cout << "Service Notification: 'Indicator' changed to : " << indicator << std::endl;
        std::cout << " ---------------------------" << std::endl;
    });

    /* Syn & Asyn call to invoke method at service */
    const char gears[] = {'P','R','N','D'};
    int arraySize = sizeof(gears) / sizeof(char);
    bool returnMessage;
    while (true) {
        // Randomly selected gears in Head Unit
        char c = gears[rand() % arraySize];
        std::string random_gear(1, c);
        // Asynchronous call to invoke me method gearSelectionHeadUnit() at service
        // std::function< void(const CommonAPI::CallStatus&, const bool&)> fcb = recv_cb;
        // myProxy->gearSelectionHeadUnitAsync(random_gear, fcb, &info);
        // Synchronous call to invoke me method gearSelectionHeadUnit() at service
        myProxy->gearSelectionHeadUnit(random_gear,callStatus,returnMessage,&info);
        std::cout << " ---------------------------" << std::endl;
        std::cout << "Result of synchronous call: " << std::endl;
        std::cout << "   callStatus: " << ((callStatus == CommonAPI::CallStatus::SUCCESS) ? "SUCCESS" : "NO_SUCCESS") << std::endl;
        std::cout << "   Input Gear:  " << random_gear << std::endl;
        std::cout << "   Output Gear Change accepted: " << returnMessage << std::endl;
        std::cout << " ---------------------------" << std::endl;
        // wait for 5 seconds
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    return 0;
}

