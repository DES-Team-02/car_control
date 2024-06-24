#pragma once

#include <CommonAPI/CommonAPI.hpp>
#include <CommonAPI/AttributeCacheExtension.hpp>
#include <v0/commonapi/JetsonProxy.hpp>
#include <mutex>

class JetsonProxyImpl
{
private:
	std::shared_ptr<typename CommonAPI::DefaultAttributeProxyHelper<v0::commonapi::JetsonProxy, CommonAPI::Extensions::AttributeCacheExtension>::class_t> _proxy;
	double _steering;
	double _throttle;
	std::mutex _mutex;
public:
	JetsonProxyImpl();
	~JetsonProxyImpl();

	bool isAvailable();
	bool subscribeSteering();
	bool subscribeThrottle();

	double getSteering();
	double getThrottle();
};