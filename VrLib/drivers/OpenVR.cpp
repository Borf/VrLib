#ifdef WIN32

#include "OpenVR.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <VrLib/Kernel.h>
#include <VrLib/Log.h>
#include <iostream>
#include <algorithm>


namespace vrlib
{
	void ProcessVREvent(const vr::VREvent_t & event)
	{
		switch (event.eventType)
		{
		case vr::VREvent_TrackedDeviceActivated:
			//SetupRenderModelForTrackedDevice(event.trackedDeviceIndex);
			logger<<"Device "<<(int)event.trackedDeviceIndex<<" attached. Setting up render model."<<Log::newline;
		break;
		case vr::VREvent_TrackedDeviceDeactivated:
			logger << "Device " << (int)event.trackedDeviceIndex << " detached." << Log::newline;
		break;
		case vr::VREvent_TrackedDeviceUpdated:
			logger<<"Device "<<(int)event.trackedDeviceIndex<<" updated."<<Log::newline;
		break;
		}
	}


	OpenVRDriver::OpenVRDriver(json::Value config)
	{
		this->config = config;

		vr::EVRInitError eError = vr::VRInitError_None;
		m_pHMD = vr::VR_Init(&eError, vr::VRApplication_Scene);

		if (eError != vr::VRInitError_None)
		{
			m_pHMD = NULL;
			logger << "Unable to init VR runtime: " << vr::VR_GetVRInitErrorAsEnglishDescription(eError) << Log::newline;
			return;
		}
		m_pRenderModels = (vr::IVRRenderModels *)vr::VR_GetGenericInterface(vr::IVRRenderModels_Version, &eError);
		if (!m_pRenderModels)
		{
			m_pHMD = NULL;
			vr::VR_Shutdown();
			logger<<"Unable to get render model interface: " << vr::VR_GetVRInitErrorAsEnglishDescription(eError) << Log::newline;
			return;
		}

		if (!vr::VRCompositor())
		{
			printf("Compositor initialization failed. See log file for details\n");
			return;
		}

		vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
		vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
		for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
		{
			if (m_pHMD->GetTrackedDeviceClass(nDevice) == vr::TrackedDeviceClass_Controller)
				controllers.push_back(nDevice);
			if (m_pHMD->GetTrackedDeviceClass(nDevice) == vr::TrackedDeviceClass_HMD)
				hmdIndex = nDevice;
			if (m_pHMD->GetTrackedDeviceClass(nDevice) == vr::TrackedDeviceClass_TrackingReference)
				lighthouses.push_back(nDevice);
		}
	}


	void OpenVRDriver::update()
	{
		if (!m_pHMD)
			return;

		vr::TrackedDevicePose_t m_rTrackedDevicePose[vr::k_unMaxTrackedDeviceCount];
		vr::VRCompositor()->WaitGetPoses(m_rTrackedDevicePose, vr::k_unMaxTrackedDeviceCount, NULL, 0);
		for (int nDevice = 0; nDevice < vr::k_unMaxTrackedDeviceCount; ++nDevice)
		{
			if (m_pHMD->GetTrackedDeviceClass(nDevice) == vr::TrackedDeviceClass_Controller && std::find(controllers.begin(), controllers.end(), nDevice) == controllers.end())
				controllers.push_back(nDevice);
			if (m_pHMD->GetTrackedDeviceClass(nDevice) == vr::TrackedDeviceClass_HMD)
				hmdIndex = nDevice;
			if (m_pHMD->GetTrackedDeviceClass(nDevice) == vr::TrackedDeviceClass_TrackingReference && std::find(lighthouses.begin(), lighthouses.end(), nDevice) == lighthouses.end())
				lighthouses.push_back(nDevice);
		}
		hmd = glm::transpose(glm::mat4(glm::make_mat3x4((float*)m_rTrackedDevicePose[hmdIndex].mDeviceToAbsoluteTracking.m)));
		
		if (controllers.size() > 0)
			controller0 = glm::transpose(glm::mat4(glm::make_mat3x4((float*)m_rTrackedDevicePose[controllers[0]].mDeviceToAbsoluteTracking.m)));
		if (controllers.size() > 1)
			controller1 = glm::transpose(glm::mat4(glm::make_mat3x4((float*)m_rTrackedDevicePose[controllers[1]].mDeviceToAbsoluteTracking.m)));

		if (lighthouses.size() > 0)
			lighthouse0 = glm::transpose(glm::mat4(glm::make_mat3x4((float*)m_rTrackedDevicePose[lighthouses[0]].mDeviceToAbsoluteTracking.m)));
		if (lighthouses.size() > 1)
			lighthouse1 = glm::transpose(glm::mat4(glm::make_mat3x4((float*)m_rTrackedDevicePose[lighthouses[1]].mDeviceToAbsoluteTracking.m)));

		// Process SteamVR events
		vr::VREvent_t event;
		while (m_pHMD->PollNextEvent(&event, sizeof(event)))
		{
			ProcessVREvent(event);
		}

		for (vr::TrackedDeviceIndex_t unDevice = 0; unDevice < vr::k_unMaxTrackedDeviceCount; unDevice++)
		{
			prevControllerStates[unDevice] = controllerStates[unDevice];
			m_pHMD->GetControllerState(unDevice, &controllerStates[unDevice]);
		}
	}




	DeviceDriverAdaptor* OpenVRDriver::getAdaptor(std::string src)
	{
		if (src == "hmd" || src == "WandLeft" || src == "WandRight")
			return new OpenVrPositionDeviceDriverAdaptor(this, src);
		else if (src.substr(0, 6) == "button")
			return new OpenVrButtonDeviceDriverAdaptor(this, src);

		logger << "Unknown OpenVR Device source: " << src << Log::newline;
		return nullptr;
	}

	OpenVRDriver::OpenVrPositionDeviceDriverAdaptor::OpenVrPositionDeviceDriverAdaptor(OpenVRDriver* driver, const std::string &config)
	{
		this->driver = driver;
		this->src = config;
	}

	glm::mat4 OpenVRDriver::OpenVrPositionDeviceDriverAdaptor::getData()
	{
		if (src == "hmd")
			return driver->hmd;
		else if (src == "WandRight")
			return driver->controller0;
		else if (src == "WandLeft")
			return driver->controller1;
		else
			logger << "Unknown OpenVR device: " << src << Log::newline;
		return glm::mat4();
	}


	OpenVRDriver::OpenVrButtonDeviceDriverAdaptor::OpenVrButtonDeviceDriverAdaptor(OpenVRDriver* driver, const std::string &config)
	{
		this->driver = driver;
		this->src = config;

		std::string btn = config.substr(11);

		id = driver->controllers[0];
		if (config.substr(6, 4) == "Left")
		{
			id = driver->controllers[1];
			btn = config.substr(10);
		}
		
			 if (btn == "Menu")		bitmask = ButtonMaskFromId(vr::k_EButton_ApplicationMenu);
		else if (btn == "Grip")		bitmask = ButtonMaskFromId(vr::k_EButton_Grip);
		else if (btn == "Touch")	bitmask = ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad);
		else if (btn == "Trigger")	bitmask = ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger);
		else
		{
			bitmask = 0;
			logger << "Unknown button: " << btn << Log::newline;
		}

	}

	DigitalState OpenVRDriver::OpenVrButtonDeviceDriverAdaptor::getData()
	{
		bool b = (driver->controllerStates[id].ulButtonPressed & bitmask) != 0;
		bool lastValue = (driver->prevControllerStates[id].ulButtonPressed & bitmask) != 0;
		if (b && !lastValue)
			return TOGGLE_ON;
		else if (b && lastValue)
			return ON;
		else if (!b && !lastValue)
			return OFF;
		else if (!b && lastValue)
			return TOGGLE_OFF;
		else
			logger << "Unknown digital device values? The universe is collapsing" << Log::newline;
	}


}

#endif