#include <thread>
#include <mutex>
#include <CommonAPI/CommonAPI.hpp>
#include <CommonAPI/AttributeCacheExtension.hpp>
#include "CanProxy.hpp"

CanProxy::CanProxy():
	_sonar({100,100,100})
{
	std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

	std::string domain = "local";
	std::string instance = "commonapi.CanTransceiver";
	std::string connection = "client-CanTransceiver";

	_proxy = runtime->buildProxyWithDefaultAttributeExtension<v0::commonapi::CanTransceiverProxy, CommonAPI::Extensions::AttributeCacheExtension>(domain, instance, connection);
	std::cout << "Waiting for service to become available." << std::endl;
	while (!_proxy->isAvailable()) {
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
	std::cout << "CAN service is available" << std::endl;
}

CanProxy::~CanProxy(){}

void CanProxy::subscribe_sonar()
{
	_proxy->getDistancesAttribute().getChangedEvent().subscribe([&](const Sonar_t& val){
		std::cout << "Sonar: "
				<< val.getSensorfrontleft() << ", "
				<< val.getSensorfrontmiddle() << ", "
				<< val.getSensorfrontright() << std::endl;
		std::lock_guard<std::mutex> lock(_mutex);
		_sonar = val;
	});
	std::cout << "Sonar subscribed" << std::endl;
}

void CanProxy::subscribe_speed()
{
	_proxy->getSpeedAttribute().getChangedEvent().subscribe([&](const unsigned int& val){
		// std::cout << "Speed: " << val << std::endl;
		{
			std::lock_guard<std::mutex> lock(_mutex);
			_speed = val;
		}
	});
	std::cout << "Speed subscribed" << std::endl;
}

unsigned int CanProxy::getSpeed()
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _speed;
}

Sonar_t CanProxy::getSonar()
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _sonar;
}