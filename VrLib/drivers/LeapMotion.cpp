#ifdef WIN32

#include "LeapMotion.h"

#include <vrlib/Log.h>

namespace vrlib
{

	LeapMotionDeviceDriver::LeapMotionDeviceDriver()
	{
		controller.addListener(*this);

	}

	DeviceDriverAdaptor* LeapMotionDeviceDriver::getAdaptor(std::string options)
	{
		return NULL;
	}

	void LeapMotionDeviceDriver::update()
	{

	}

	void LeapMotionDeviceDriver::onInit(const Leap::Controller&)
	{
		logger << "Leap Initialized" << Log::newline;
	}
	void LeapMotionDeviceDriver::onFrame(const Leap::Controller&)
	{
		logger << "Leap FRAME!" << Log::newline;
	}

	void LeapMotionDeviceDriver::onConnect(const Leap::Controller&)				{	logger << "Leap Connect" << Log::newline;	}
	void LeapMotionDeviceDriver::onDisconnect(const Leap::Controller&)			{	logger << "Leap Disconnect" << Log::newline;	}
	void LeapMotionDeviceDriver::onExit(const Leap::Controller&)				{	logger << "Leap exit" << Log::newline;	}
	void LeapMotionDeviceDriver::onFocusGained(const Leap::Controller&)			{	logger << "Leap Gained Focus" << Log::newline;	}
	void LeapMotionDeviceDriver::onFocusLost(const Leap::Controller&)			{	logger << "Leap Lost Focus" << Log::newline;	}
	void LeapMotionDeviceDriver::onDeviceChange(const Leap::Controller&)		{	logger << "Leap Devicechange" << Log::newline;	}
	void LeapMotionDeviceDriver::onServiceConnect(const Leap::Controller&)		{	logger<<"Leap Service connected"<<Log::newline;	}
	void LeapMotionDeviceDriver::onServiceDisconnect(const Leap::Controller&)	{	logger<<"Leap Service disconnected"<< Log::newline;	}
}



#endif