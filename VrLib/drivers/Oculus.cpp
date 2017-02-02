#ifdef WIN32


#include "Oculus.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <VrLib\Kernel.h>
#include <VrLib\Log.h>
#include <iostream>
#include <OVR_CAPI_GL.h>
#include <Extras/OVR_Math.h>

namespace vrlib
{
	static void quat_to_matrix(const float *quat, float *mat);


	OculusDeviceDriver::OculusDeviceDriver(json config)
	{
		this->config = config;

		ovrResult result = ovr_Initialize(nullptr);
		if (!OVR_SUCCESS(result))
			logger << "Failed to initialize libOVR." << Log::newline;


		ovrGraphicsLuid luid;
		result = ovr_Create(&hmd, &luid);
		if (!OVR_SUCCESS(result))
		{
			logger << "Could not create ovr" << Log::newline;
			return;
		}
	}

	void OculusDeviceDriver::update(KeyboardDeviceDriver* keyboardDriver)
	{
		ovrVector3f               ViewOffset[2] = { { 0,0,0 },{ 0,0,0 } };
		ovrPosef                  EyeRenderPose[2];
		double           ftiming = ovr_GetPredictedDisplayTime(hmd, 0);
		double           sensorSampleTime = ovr_GetTimeInSeconds();
		ovrTrackingState hmdState = ovr_GetTrackingState(hmd, ftiming, ovrTrue);
		ovr_CalcEyePoses(hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose);
		static OVR::Vector3f Pos2(0.0f, 1.6f, -5.0f);


		// Get view and projection matrices
		OVR::Matrix4f rollPitchYaw = OVR::Matrix4f::RotationY(0);// Yaw);
		OVR::Matrix4f finalRollPitchYaw = rollPitchYaw * OVR::Matrix4f(EyeRenderPose[0].Orientation);
		OVR::Vector3f finalUp = finalRollPitchYaw.Transform(OVR::Vector3f(0, 1, 0));
		OVR::Vector3f finalForward = finalRollPitchYaw.Transform(OVR::Vector3f(0, 0, -1));
		OVR::Vector3f shiftedEyePos = Pos2 + rollPitchYaw.Transform(EyeRenderPose[0].Position);

		OVR::Matrix4f view = OVR::Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
		float quat[] = { EyeRenderPose[0].Orientation.x, EyeRenderPose[0].Orientation.y, EyeRenderPose[0].Orientation.z, EyeRenderPose[0].Orientation.w };
		float rot_mat[16];
		quat_to_matrix(quat, rot_mat);

		glm::mat4 modelviewMatrix;
		modelviewMatrix *= glm::make_mat4(rot_mat);
		modelviewMatrix = glm::translate(modelviewMatrix, glm::vec3(-EyeRenderPose->Position.x, -EyeRenderPose->Position.y, -EyeRenderPose->Position.z));
		modelviewMatrix = glm::translate(modelviewMatrix, glm::vec3(0, -1.5f, -0.5f));

		headMatrix = glm::inverse(modelviewMatrix);
	}

	DeviceDriverAdaptor* OculusDeviceDriver::getAdaptor(std::string options)
	{
		OculusDeviceDriverAdaptor* adaptor = new OculusDeviceDriverAdaptor(this, options);
		return adaptor;

	}

	OculusDeviceDriver::OculusDeviceDriverAdaptor::OculusDeviceDriverAdaptor(OculusDeviceDriver* driver, const std::string &src)
	{
		this->driver = driver;
	}


	static void quat_to_matrix(const float *quat, float *mat)
	{
		mat[0] = 1.0f - 2.0f * quat[1] * quat[1] - 2.0f * quat[2] * quat[2];
		mat[4] = 2.0f * quat[0] * quat[1] + 2.0f * quat[3] * quat[2];
		mat[8] = 2.0f * quat[2] * quat[0] - 2.0f * quat[3] * quat[1];
		mat[12] = 0.0f;

		mat[1] = 2.0f * quat[0] * quat[1] - 2.0f * quat[3] * quat[2];
		mat[5] = 1.0f - 2.0f * quat[0] * quat[0] - 2.0f * quat[2] * quat[2];
		mat[9] = 2.0f * quat[1] * quat[2] + 2.0f * quat[3] * quat[0];
		mat[13] = 0.0f;

		mat[2] = 2.0f * quat[2] * quat[0] + 2.0f * quat[3] * quat[1];
		mat[6] = 2.0f * quat[1] * quat[2] - 2.0f * quat[3] * quat[0];
		mat[10] = 1.0f - 2.0f * quat[0] * quat[0] - 2.0f * quat[1] * quat[1];
		mat[14] = 0.0f;

		mat[3] = mat[7] = mat[11] = 0.0f;
		mat[15] = 1.0f;
	}
	glm::mat4 OculusDeviceDriver::OculusDeviceDriverAdaptor::getData()
	{
		return driver->headMatrix;
	}

}



#endif