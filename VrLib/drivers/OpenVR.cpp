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



	}

	DeviceDriverAdaptor* OpenVRDriver::getAdaptor(std::string options)
	{
/*		ovrResult result = ovr_Initialize(nullptr);
		if (!OVR_SUCCESS(result))
			logger << "Failed to initialize libOVR." << Log::newline;

		OculusDeviceDriverAdaptor* adaptor = new OculusDeviceDriverAdaptor(config["Settings"]);
		hmd = &adaptor->hmd;
		adaptor->eye_rdesc = &eye_rdesc[0];
		return adaptor;*/
		return new OpenVrDeviceDriverAdaptor("");
	}

	OpenVRDriver::OpenVrDeviceDriverAdaptor::OpenVrDeviceDriverAdaptor(const std::string &config)
	{

	}



}