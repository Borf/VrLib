#pragma once

#include <VrLib/Device.h>

#include <list>
#include <vrlib/json.h>

namespace vr {
	class IVRSystem;
	class IVRRenderModels;
}

namespace vrlib
{
	class KeyboardDeviceDriver;

	class OpenVRDriver : public DeviceDriver
	{
		class OpenVrDeviceDriverAdaptor : public PositionalDeviceDriverAdaptor
		{
		public:
			OpenVrDeviceDriverAdaptor(const std::string &config);
			glm::mat4 getData() { return glm::mat4(); }
			
		};

		json::Value config;

	public:
		vr::IVRSystem *m_pHMD;
		vr::IVRRenderModels *m_pRenderModels;


		OpenVRDriver(json::Value config);
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);
	};
}