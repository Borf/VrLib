#include "Oculus.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <VrLib\Kernel.h>
#include <VrLib\Log.h>
#include <iostream>

namespace vrlib
{
	OculusDeviceDriver::OculusDeviceDriver(json::Value config)
	{
		this->showConfigPanel = false;
		this->config = config;

//		Json::Value::Members configMembers = config.getMemberNames();
		/*for (size_t i = 0; i < configMembers.size(); i++)
		{
		static struct ActionMapping { std::string str; Action action; } actionMapping[] = {
		{ "reset", RESET },
		{ "inceyedist", INCREASE_EYE_DISTANCE },
		{ "deceyedist", DECREASE_EYE_DISTANCE },
		{ "toggleconf", TOGGLE_CONFIG }
		};

		for (int ii = 0; ii < sizeof(actionMapping) / sizeof(ActionMapping); ii++)
		if (config[configMembers[i]].isMember(actionMapping[ii].str))
		keyHandlers[configMembers[i]].push_back(keyhandler(actionMapping[ii].action, KeyboardDeviceDriver::parseString(config[configMembers[i]][actionMapping[ii].str].asString())));
		}*/
	}

	void OculusDeviceDriver::update(KeyboardDeviceDriver* keyboardDriver)
	{
		
	}

	DeviceDriverAdaptor* OculusDeviceDriver::getAdaptor(std::string options)
	{
		ovrResult result = ovr_Initialize(nullptr);
		if (!OVR_SUCCESS(result))
			logger << "Failed to initialize libOVR." << Log::newline;

		OculusDeviceDriverAdaptor* adaptor = new OculusDeviceDriverAdaptor(config["Settings"]);
		hmd = &adaptor->hmd;
		adaptor->eye_rdesc = &eye_rdesc[0];
		return adaptor;

	}

	OculusDeviceDriver::OculusDeviceDriverAdaptor::OculusDeviceDriverAdaptor(json::Value config)
	{
		ovrGraphicsLuid luid;
		ovrResult result = ovr_Create(&hmd, &luid);
		if (!OVR_SUCCESS(result))
		{
			logger << "Could not create ovr" << Log::newline;
			return;
		}
	}


	glm::vec3 OculusDeviceDriver::OculusDeviceDriverAdaptor::getDirection()
	{
		//return getEulerAngles(sensorFusion.GetPredictedOrientation());
		return glm::vec3();
	}

	void quat_to_matrix(const float *quat, float *mat);

	glm::mat4 OculusDeviceDriver::OculusDeviceDriverAdaptor::getData()
	{
		/*	if (useTracker) {
				return glm::mat4_cast(glm::quat(getEulerAngles(sensorFusion.GetPredictedOrientation())));
				}
				*/


/*		ovrPosef pose;
		pose = ovrHmd_GetEyePose(hmd, ovrEye_Left);

		glm::mat4 ret = glm::translate(glm::mat4(), glm::vec3(eye_rdesc[ovrEye_Left].ViewAdjust.x, eye_rdesc[ovrEye_Left].ViewAdjust.y, eye_rdesc[ovrEye_Left].ViewAdjust.z));
		glm::mat4 rotMatrix;
		quat_to_matrix(&pose.Orientation.x, glm::value_ptr(rotMatrix));


		return ret * rotMatrix;*/

		return glm::translate(glm::mat4(), glm::vec3(0, 0, 0));
	}

	void OculusDeviceDriver::OculusDeviceDriverAdaptor::resetSensor()
	{
		//sensorFusion.Reset();
	}


}