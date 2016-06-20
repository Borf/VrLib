#pragma once

#include <VrLib/Device.h>

#include <list>
#include <vrlib/json.h>
#include <openvr.h>



namespace vrlib
{
	class KeyboardDeviceDriver;

	class OpenVRDriver : public DeviceDriver
	{
		class OpenVrPositionDeviceDriverAdaptor : public PositionalDeviceDriverAdaptor
		{
			std::string src;
			OpenVRDriver* driver;
		public:
			OpenVrPositionDeviceDriverAdaptor(OpenVRDriver* driver, const std::string &config);
			glm::mat4 getData();
		};

		class OpenVrButtonDeviceDriverAdaptor : public DigitalDeviceDriverAdaptor
		{
			std::string src;
			OpenVRDriver* driver;
			int id;
			uint64_t bitmask;
		public:
			OpenVrButtonDeviceDriverAdaptor(OpenVRDriver* driver, const std::string &config);
			DigitalState getData();
		};



		json::Value config;

	public:
		vr::IVRSystem *m_pHMD;
		vr::IVRRenderModels *m_pRenderModels;
		
		std::vector<int> controllers;
		int hmdIndex;
		std::vector<int> lighthouses;

		vr::VRControllerState_t prevControllerStates[16];
		vr::VRControllerState_t controllerStates[16];

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