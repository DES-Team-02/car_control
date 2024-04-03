#include <CommonAPI/CommonAPI.hpp>
#include <CommonAPI/AttributeCacheExtension.hpp>
#include <v0/commonapi/CanTransceiverProxy.hpp>
#include <mutex>

typedef v0::commonapi::CanTransceiver::SonarArrayStruct Sonar_t;

class CanProxy
{
private:
	std::shared_ptr<typename CommonAPI::DefaultAttributeProxyHelper<v0::commonapi::CanTransceiverProxy, CommonAPI::Extensions::AttributeCacheExtension>::class_t> _proxy;
	unsigned int _speed;
	Sonar_t 	_sonar;
	std::mutex	_mutex;
public:
	CanProxy();
	~CanProxy();

	void subscribe_speed();
	void subscribe_sonar();

	unsigned int getSpeed();
	Sonar_t		 getSonar();
};