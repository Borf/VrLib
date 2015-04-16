#include "Oculus.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <VrLib\Kernel.h>
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
		/*for (std::map<std::string, std::list<keyhandler> >::iterator it = keyHandlers.begin(); it != keyHandlers.end(); it++)
		{
		for (std::list<keyhandler>::iterator it2 = it->second.begin(); it2 != it->second.end(); it2++)
		{
		keyhandler& kh = *it2;

		if (keyboardDriver->isPressed(kh.second.first) && keyboardDriver->isModPressed(kh.second.second))
		{
		switch (kh.first)
		{
		case RESET:
		resetSensor();
		break;
		case INCREASE_EYE_DISTANCE:
		increaseEyeDistance(0.0002f);
		break;
		case DECREASE_EYE_DISTANCE:
		increaseEyeDistance(-0.0002f);
		break;
		case TOGGLE_CONFIG:
		toggleConfigPanel(true);
		break;
		default:
		toggleConfigPanel(false);
		break;
		}
		}
		else
		toggleConfigPanel(false);
		}
		}*/
	}

	DeviceDriverAdaptor* OculusDeviceDriver::getAdaptor(std::string options)
	{
		ovr_Initialize();

		OculusDeviceDriverAdaptor* adaptor = new OculusDeviceDriverAdaptor(config["Settings"]);
		hmd = &adaptor->hmd;
		adaptor->eye_rdesc = &eye_rdesc[0];
		return adaptor;


		/*OVR::System::Init();
		OculusDeviceDriverAdaptor* adaptor = new OculusDeviceDriverAdaptor(config["Settings"]);
		hmdInfo = &adaptor->hmdInfo;
		stereoConfig = &adaptor->stereoConfig;
		return adaptor;*/
	}

	OculusDeviceDriver::OculusDeviceDriverAdaptor::OculusDeviceDriverAdaptor(json::Value config)
	{
		hmd = ovrHmd_Create(0);
		if (!hmd)
		{
			printf("No HMD found");
			hmd = ovrHmd_CreateDebug(ovrHmd_DK1);
		}

		ovrHmd_SetEnabledCaps(hmd, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);
		ovrHmd_ConfigureTracking(hmd, ovrTrackingCap_Orientation | ovrTrackingCap_MagYawCorrection | ovrTrackingCap_Position, 0);
	}


	/*static glm::vec3 getEulerAngles(OVR::Quatf & in) {
		glm::vec3 eulerAngles;
		in.GetEulerAngles<
		OVR::Axis_X, OVR::Axis_Y, OVR::Axis_Z,
		OVR::Rotate_CW, OVR::Handed_R
		>(&eulerAngles.x, &eulerAngles.y, &eulerAngles.z);
		return eulerAngles;
		}*/

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


		ovrPosef pose;
		pose = ovrHmd_GetEyePose(hmd, ovrEye_Left);

		glm::mat4 ret = glm::translate(glm::mat4(), glm::vec3(eye_rdesc[ovrEye_Left].ViewAdjust.x, eye_rdesc[ovrEye_Left].ViewAdjust.y, eye_rdesc[ovrEye_Left].ViewAdjust.z));
		glm::mat4 rotMatrix;
		quat_to_matrix(&pose.Orientation.x, glm::value_ptr(rotMatrix));


		return ret * rotMatrix;

		return glm::translate(glm::mat4(), glm::vec3(0, 0, 0));
	}

	void OculusDeviceDriver::OculusDeviceDriverAdaptor::resetSensor()
	{
		//sensorFusion.Reset();
	}

	void OculusDeviceDriver::OculusDeviceDriverAdaptor::increaseEyeDistance(float distanceToAdd)
	{
		/*float newInterpupillaryDistance = hmdInfo.InterpupillaryDistance + distanceToAdd;

		if (newInterpupillaryDistance < minInterpupillaryDistance)
		newInterpupillaryDistance = minInterpupillaryDistance;
		else if (newInterpupillaryDistance > maxInterpupillaryDistance)
		newInterpupillaryDistance = maxInterpupillaryDistance;

		int tmpValue = newInterpupillaryDistance * 10000;
		newInterpupillaryDistance = tmpValue / 10000.0f;

		hmdInfo.InterpupillaryDistance = newInterpupillaryDistance;
		stereoConfig.SetHMDInfo(hmdInfo);*/
	}

	float OculusDeviceDriver::OculusDeviceDriverAdaptor::getEyeDistance()
	{
		return 0;
		//return hmdInfo.InterpupillaryDistance;
	}

	glm::vec3 OculusDeviceDriver::getDirection()
	{
		return glm::vec3();
		//return ((OculusDeviceDriverAdaptor*)Kernel::getInstance()->getDeviceDriverAdaptor("MainUserHead"))->getDirection();
	}

	void OculusDeviceDriver::resetSensor()
	{
		//((OculusDeviceDriverAdaptor*)Kernel::getInstance()->getDeviceDriverAdaptor("MainUserHead"))->resetSensor();
	}

	void OculusDeviceDriver::increaseEyeDistance(float distanceToAdd)
	{
		/*toggleConfigPanel(true);
		((OculusDeviceDriverAdaptor*)Kernel::getInstance()->getDeviceDriverAdaptor("MainUserHead"))->increaseEyeDistance(distanceToAdd);*/
	}

	float OculusDeviceDriver::getEyeDistance()
	{
		//return ((OculusDeviceDriverAdaptor*)Kernel::getInstance()->getDeviceDriverAdaptor("MainUserHead"))->getEyeDistance();
		return 0;
	}

	void OculusDeviceDriver::toggleConfigPanel(bool value)
	{
		this->showConfigPanel = value;
	}

	bool OculusDeviceDriver::configPanelIsShown()
	{
		return this->showConfigPanel;
	}

	void OculusDeviceDriver::beginFrame()
	{
		/* the drawing starts with a call to ovrHmd_BeginFrame */
		ovrHmd_BeginFrame(*hmd, 0);
	}

}