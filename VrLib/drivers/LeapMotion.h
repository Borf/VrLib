#pragma once

#ifdef WIN32

#include <VrLib/Device.h>
#include <Leap.h>

namespace vrlib
{

	class LeapMotionDeviceDriver : public DeviceDriver, public Leap::Listener
	{
	private:
		Leap::Controller controller;
		virtual void onInit(const Leap::Controller&);
		virtual void onConnect(const Leap::Controller&);
		virtual void onDisconnect(const Leap::Controller&);
		virtual void onExit(const Leap::Controller&);
		virtual void onFrame(const Leap::Controller&);
		virtual void onFocusGained(const Leap::Controller&);
		virtual void onFocusLost(const Leap::Controller&);
		virtual void onDeviceChange(const Leap::Controller&);
		virtual void onServiceConnect(const Leap::Controller&);
		virtual void onServiceDisconnect(const Leap::Controller&);

	public:
		LeapMotionDeviceDriver();
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);
		virtual void update();

	};

}


#endif