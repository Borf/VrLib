#include "OpenVR.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <VrLib\Kernel.h>
#include <VrLib\Log.h>
#include <iostream>

#include <openvr.h>


namespace vrlib
{
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
	}




	DeviceDriverAdaptor* OpenVRDriver::getAdaptor(std::string options)
	{
		return new OpenVrDeviceDriverAdaptor(this, options);
	}

	OpenVRDriver::OpenVrDeviceDriverAdaptor::OpenVrDeviceDriverAdaptor(OpenVRDriver* driver, const std::string &config)
	{
		this->driver = driver;
		this->src = config;
	}

	glm::mat4 OpenVRDriver::OpenVrDeviceDriverAdaptor::getData()
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



}