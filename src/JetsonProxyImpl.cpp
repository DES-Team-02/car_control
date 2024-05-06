#include <thread>
#include <mutex>
#include <CommonAPI/CommonAPI.hpp>
#include <CommonAPI/AttributeCacheExtension.hpp>
#include "JetsonProxyImpl.hpp"

JetsonProxyImpl::JetsonProxyImpl():
	_steering(0),
	_throttle(0)
{
	std::shared_ptr<CommonAPI::Runtime> runtime = CommonAPI::Runtime::get();

	std::string domain = "local";
	std::string instance = "commonapi.Jetson";
	std::string connection = "client-Jetson";

	_proxy = runtime->buildProxyWithDefaultAttributeExtension<v0::commonapi::JetsonProxy, CommonAPI::Extensions::AttributeCacheExtension>(domain, instance, connection);
}

JetsonProxyImpl::~JetsonProxyImpl(){}

bool JetsonProxyImpl::isAvailable()
{
	return _proxy->isAvailable();
}

bool JetsonProxyImpl::subscribeSteering()
{
	if (!_proxy->isAvailable())
		return false;
	_proxy->getSteeringAttribute().getChangedEvent().subscribe([&](const double& val){
		std::cout << "Steering: " << val << std::endl;
		std::lock_guard<std::mutex> lock(_mutex);
		_steering = val;
	});
	std::cout << "Steering subscribed";
	return true;
}

bool JetsonProxyImpl::subscribeThrottle()
{
	if (!_proxy->isAvailable())
		return false;
	_proxy->getThrottleAttribute().getChangedEvent().subscribe([&](const double& val){
		std::cout << "Throttle: " << val << std::endl;
		std::lock_guard<std::mutex> lock(_mutex);
		_throttle = val;
	});
	std::cout << "Throttle subscribed";
	return true;
}

double JetsonProxyImpl::getSteering()
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _steering;
}

double JetsonProxyImpl::getThrottle()
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _throttle;
}