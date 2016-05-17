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
			std::string src;
			OpenVRDriver* driver;
		public:
			OpenVrDeviceDriverAdaptor(OpenVRDriver* driver, const std::string &config);
			glm::mat4 getData();
			
		};

		json::Value config;

	public:
		vr::IVRSystem *m_pHMD;
		vr::IVRRenderModels *m_pRenderModels;
		
		std::vector<int> controllers;
		int hmdIndex;
		std::vector<int> lighthouses;


		glm::mat4 hmd;
		glm::mat4 controller0;
		glm::mat4 controller1;
		glm::mat4 lighthouse0;
		glm::mat4 lighthouse1;


		OpenVRDriver(json::Value config);
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);
		virtual void update();
	};
}