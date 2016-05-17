#define NOMINMAX

#include "RiftViewport.h"

#include <VrLib/Application.h>
#include <VrLib/User.h>
#include <VrLib/drivers/Oculus.h>
#include <VrLib/Kernel.h>

#include <VrLib/gl/FBO.h>
#include <VrLib/gl/Shader.h>
#include <VrLib/Log.h>

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/wglew.h>
#include <OVR_CAPI_GL.h>
#include <Extras/OVR_Math.h>

//OVR_EXPORT void ovrhmd_EnableHSWDisplaySDKRender(ovrHmd hmd, ovrBool enable);


namespace vrlib
{

	/* convert a quaternion to a rotation matrix */
	void quat_to_matrix(const float *quat, float *mat)
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


	int fb_width, fb_height;

	RiftViewport::RiftViewport(User* user, OculusDeviceDriver* oculusDriver, Kernel* kernel) : Viewport(user)
	{
		mirrorTexture = nullptr;
		mirrorFBO = 0;

		ovrResult result;

		this->eye = eye;
		this->oculusDriver = oculusDriver;

		ovrHmd hmd = *oculusDriver->hmd;
		hmdDesc = ovr_GetHmdDesc(hmd);
		ovrSizei windowSize = { hmdDesc.Resolution.w / 2, hmdDesc.Resolution.h / 2 };
		logger << "Window size: " << windowSize.w << ", " << windowSize.h << Log::newline;


		// Make eye render buffers
		for (int eye = 0; eye < 2; ++eye)
		{
			ovrSizei idealTextureSize = ovr_GetFovTextureSize(hmd, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye], 1);
			eyeRenderTexture[eye] = new TextureBuffer(hmd, true, true, idealTextureSize, 1, NULL, 1);
			eyeDepthBuffer[eye] = new DepthBuffer(eyeRenderTexture[eye]->GetSize(), 0);

			if (!eyeRenderTexture[eye]->TextureSet)
			{
				logger<<"Failed to create texture."<<Log::newline;
			}
		}

		// Create mirror texture and an FBO used to copy mirror texture to back buffer
		result = ovr_CreateMirrorTextureGL(hmd, GL_SRGB8_ALPHA8, windowSize.w, windowSize.h, reinterpret_cast<ovrTexture**>(&mirrorTexture));
		if (!OVR_SUCCESS(result))
		{
			logger << "Failed to create mirror texture." << Log::newline;
		}

		// Configure the mirror read buffer
		glGenFramebuffers(1, &mirrorFBO);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mirrorTexture->OGL.TexId, 0);
		glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);


		EyeRenderDesc[0] = ovr_GetRenderDesc(hmd, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
		EyeRenderDesc[1] = ovr_GetRenderDesc(hmd, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);

		// Turn off vsync to let the compositor do its magic
		wglSwapIntervalEXT(0);

	}

	glm::mat4 RiftViewport::getProjectionMatrix()
	{
		return glm::perspective(glm::radians(60.0f), 1.0f, 0.001f, 1000.0f);
	}


	float dist = 0;

	void RiftViewport::draw(Application* application)
	{
		ovrHmd hmd = *oculusDriver->hmd;
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		float rot_mat[16];


		// Get eye poses, feeding in correct IPD offset
		ovrVector3f               ViewOffset[2] = { EyeRenderDesc[0].HmdToEyeViewOffset,
			EyeRenderDesc[1].HmdToEyeViewOffset };
		ovrPosef                  EyeRenderPose[2];
		double           ftiming = ovr_GetPredictedDisplayTime(hmd, 0);
		// Keeping sensorSampleTime as close to ovr_GetTrackingState as possible - fed into the layer
		double           sensorSampleTime = ovr_GetTimeInSeconds();
		ovrTrackingState hmdState = ovr_GetTrackingState(hmd, ftiming, ovrTrue);
		ovr_CalcEyePoses(hmdState.HeadPose.ThePose, ViewOffset, EyeRenderPose);
		for (int eye = 0; eye < 2; eye++)
		{
			// Increment to use next texture, just before writing
			eyeRenderTexture[eye]->TextureSet->CurrentIndex = (eyeRenderTexture[eye]->TextureSet->CurrentIndex + 1) % eyeRenderTexture[eye]->TextureSet->TextureCount;

			// Switch to eye render target
			eyeRenderTexture[eye]->SetAndClearRenderSurface(eyeDepthBuffer[eye]);

			static OVR::Vector3f Pos2(0.0f, 1.6f, -5.0f);
			Pos2.y = ovr_GetFloat(hmd, OVR_KEY_EYE_HEIGHT, Pos2.y);

			// Get view and projection matrices
			OVR::Matrix4f rollPitchYaw = OVR::Matrix4f::RotationY(0);// Yaw);
			OVR::Matrix4f finalRollPitchYaw = rollPitchYaw * OVR::Matrix4f(EyeRenderPose[eye].Orientation);
			OVR::Vector3f finalUp = finalRollPitchYaw.Transform(OVR::Vector3f(0, 1, 0));
			OVR::Vector3f finalForward = finalRollPitchYaw.Transform(OVR::Vector3f(0, 0, -1));
			OVR::Vector3f shiftedEyePos = Pos2 + rollPitchYaw.Transform(EyeRenderPose[eye].Position);

			OVR::Matrix4f view = OVR::Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
			OVR::Matrix4f proj = ovrMatrix4f_Projection(hmdDesc.DefaultEyeFov[eye], 0.2f, 500.0f, ovrProjection_RightHanded);

			glm::mat4 projectionMatrix(proj.M[0][0],
				proj.M[0][1],
				proj.M[0][2],
				proj.M[0][3],
				proj.M[0][4],
				proj.M[0][5],
				proj.M[0][6],
				proj.M[0][7],
				proj.M[0][8],
				proj.M[0][9],
				proj.M[0][10],
				proj.M[0][11],
				proj.M[0][12],
				proj.M[0][13],
				proj.M[0][14],
				proj.M[0][15]
				);

			float quat[] = { EyeRenderPose[eye].Orientation.x, EyeRenderPose[eye].Orientation.y, EyeRenderPose[eye].Orientation.z, EyeRenderPose[eye].Orientation.w };
			quat_to_matrix(quat, rot_mat);

			projectionMatrix = glm::transpose(projectionMatrix);


			glm::mat4 modelviewMatrix;
			modelviewMatrix *= glm::make_mat4(rot_mat);
			modelviewMatrix = glm::translate(modelviewMatrix, glm::vec3(-EyeRenderPose->Position.x, -EyeRenderPose->Position.y, -EyeRenderPose->Position.z));
			modelviewMatrix = glm::translate(modelviewMatrix, glm::vec3(0, 0, -0.5f));
			user->matrix = glm::inverse(modelviewMatrix);

			resetOpenGL();


			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			glLoadMatrixf(glm::value_ptr(projectionMatrix));
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glLoadMatrixf(glm::value_ptr(modelviewMatrix));




			application->draw(projectionMatrix, modelviewMatrix, user->matrix);
			// Avoids an error when calling SetAndClearRenderSurface during next iteration.
			// Without this, during the next while loop iteration SetAndClearRenderSurface
			// would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
			// associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
			eyeRenderTexture[eye]->UnsetRenderSurface();

		}

		// Do distortion rendering, Present and flush/sync

		// Set up positional data.
		ovrViewScaleDesc viewScaleDesc;
		viewScaleDesc.HmdSpaceToWorldScaleInMeters = 1.0f;
		viewScaleDesc.HmdToEyeViewOffset[0] = ViewOffset[0];
		viewScaleDesc.HmdToEyeViewOffset[1] = ViewOffset[1];

		ovrLayerEyeFov ld;
		ld.Header.Type = ovrLayerType_EyeFov;
		ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.

		for (int eye = 0; eye < 2; ++eye)
		{
			ld.ColorTexture[eye] = eyeRenderTexture[eye]->TextureSet;
			ld.Viewport[eye] = ovrRecti{ ovrVector2i {0,0}, eyeRenderTexture[eye]->GetSize() };
			ld.Fov[eye] = hmdDesc.DefaultEyeFov[eye];
			ld.RenderPose[eye] = EyeRenderPose[eye];
			ld.SensorSampleTime = sensorSampleTime;
		}

		ovrLayerHeader* layers = &ld.Header;
		ovrResult result = ovr_SubmitFrame(hmd, 0, &viewScaleDesc, &layers, 1);
		// exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
		if (!OVR_SUCCESS(result))
		{
			logger << "Error submitting frame" << Log::newline;
			return;
		}

		bool isVisible = (result == ovrSuccess);

		// Blit mirror texture to back buffer
		glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		GLint w = mirrorTexture->OGL.Header.TextureSize.w;
		GLint h = mirrorTexture->OGL.Header.TextureSize.h;
		glBlitFramebuffer(0, h, w, 0,
			0, 0, w, h,
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

		assert(glGetError() == GL_NO_ERROR);


	}
}