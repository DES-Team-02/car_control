#include "CanReceiverProxy.hpp"

CanProxy::CanProxy()
{
	_buildProxy();
	_subscribeSpeed();
	_subscribeSonar();
}

CanProxy::~CanProxy(){}

void CanProxy::_buildProxy()
{
	std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

	std::string domain = "local";
	std::string instance = "commonapi.CanReceiver";
	std::string connection = "car-control";

	_proxy = runtime->buildProxyWithDefaultAttributeExtension\
		<v0::commonapi::CanReceiverProxy, \
		CommonAPI::Extensions::AttributeCacheExtension>\(domain, instance, connection);

	std::cout << "Waiting for service to become available." << std::endl;
	while (!_proxy->isAvailable()) {
		std::this_thread::sleep_for(std::chrono::microseconds(10));
	}
	std::cout << "CAN service is available" << std::endl;
}

void _subscribeSpeed()
{
	_proxy->getSpeedAttribute().getChangedEvent().subscribe([&](const int& val){
		//std::cout << "Received speed: " << val << std::endl;
		_speed = val;
	});
	std::cout << "SpeedSensor subscribed" << std::endl;
}

void _subscribeSonar()
{
	_proxy->getDistancesAttribute().getChangedEvent().subscribe([&](const sonarArr_t val){
		_sonar = val;
	});
	std::cout << "SonarSensor subscribed" << std::endl;
}

unsigned int getSpeed() { return _speed; }

sonarArr_t getSonar() { return _sonar; }