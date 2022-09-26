#pragma once

#ifdef WIN32

#include <VrLib/Device.h>

#include <list>
#include <VrLib/json.hpp>
#include <openvr.h>



namespace vrlib
{
	class KeyboardDeviceDriver;

	class OpenVRDriver : public DeviceDriver
	{
	public:
		glm::mat4 hmd;
		glm::mat4 controller0;
		glm::mat4 controller1;
	private:
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

		class OpenVrTouchDeviceDriverAdaptor : public TwoDimensionDeviceDriverAdaptor
		{
			std::string src;
			OpenVRDriver* driver;
			int id;
			uint64_t bitmask;
		public:
			OpenVrTouchDeviceDriverAdaptor(OpenVRDriver* driver, const std::string &config);
			glm::vec2 getData();
		};


		nlohmann::json config;

	public:
		vr::IVRSystem *m_pHMD;
		vr::IVRRenderModels *m_pRenderModels;
		
		std::vector<int> controllers;
		int hmdIndex;
		std::vector<int> lighthouses;

		vr::VRControllerState_t prevControllerStates[64];
		vr::VRControllerState_t controllerStates[64];

		glm::mat4 lighthouse0;
		glm::mat4 lighthouse1;

		glm::vec2 touch0;
		glm::vec2 touch1;


		OpenVRDriver(nlohmann::json config);
		virtual DeviceDriverAdaptor* getAdaptor(std::string options);
		virtual void update();
	};
}

#else

#include <VrLib/Device.h>

namespace vrlib
{
	class OpenVRDriver : public DeviceDriver
	{
		OpenVRDriver(json config) {}
		virtual DeviceDriverAdaptor* getAdaptor(std::string options) { return nullptr; }
		virtual void update() {};
	
	}

}

#endif