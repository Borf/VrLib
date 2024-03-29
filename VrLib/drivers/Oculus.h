#pragma once

#include <VrLib/Device.h>
#include <VrLib/drivers/Keyboard.h>

#include <list>
#include <VrLib/json.hpp>

#ifdef WIN32
#include <OVR_CAPI.h>

namespace vrlib
{
	class KeyboardDeviceDriver;

	class OculusDeviceDriver : public DeviceDriver
	{
		class OculusDeviceDriverAdaptor : public PositionalDeviceDriverAdaptor
		{
			OculusDeviceDriver* driver;
		public:
			OculusDeviceDriverAdaptor(OculusDeviceDriver* driver, const std::string &src);
			virtual glm::mat4 getData();
		};

		nlohmann::json config;
	public:
		OculusDeviceDriver(nlohmann::json config);
		void update(KeyboardDeviceDriver* keyboardDriver);
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);

		ovrHmd hmd;
		ovrEyeRenderDesc eye_rdesc[2];

		glm::mat4 headMatrix;
	};
}

#else


namespace vrlib
{
	class OculusDeviceDriver : public DeviceDriver
	{
	
	
	};
}


#endif