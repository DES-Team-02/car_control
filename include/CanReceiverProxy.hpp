#include <CommonAPI/CommonAPI.hpp>
#include <CommonAPI/AttributeCacheExtension.hpp>
#include <v0/commonapi/CanTranceiverProxy.hpp>

typedef v0::commonapi::CanReceiver::SonarArrayStruct sonarArr_t;

class CanProxy()
{
private:
	std::shared_ptr<typename CommonAPI::DefaultAttributeProxyHelper \
		<v0::commonapi::CanTranceiverProxy, \
		CommonAPI::Extensions::AttributeCacheExtension>::class_t> _proxy;
	unsigned int	_speed;
	sonarArr_t		_sonar;

	void			_buildProxy();
	void			_subscribeSpeed();
	void			_subscribeSonar();
public:
	CanProxy();
	~CanProxy();

	unsigned int	getSpeed();
	sonarArr_t		getSonar();
}