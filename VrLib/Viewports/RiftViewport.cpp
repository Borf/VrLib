#define NOMINMAX

#include "RiftViewport.h"

#include <VrLib/Application.h>
#include <VrLib/User.h>
#include <VrLib/drivers/Oculus.h>
#include <VrLib/Kernel.h>

#include <VrLib/gl/FBO.h>
#include <VrLib/gl/Shader.h>

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define OVR_OS_WIN32
#include <OVR_CAPI.h>
#include <OVR_CAPI_GL.h>
#include <CAPI/CAPI_HMDState.h>
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


	// Renders textures to frame buffer
	void ovrHmd_EndFrame_noswap(ovrHmd hmddesc,
		const ovrPosef renderPose[2],
		const ovrTexture eyeTexture[2])
	{
		using namespace OVR::CAPI;

		HMDState* hmds = (HMDState*)hmddesc->Handle;
		if (!hmds) return;

		hmds->SubmitEyeTextures(renderPose, eyeTexture);

		// Debug state checks: Must be in BeginFrame, on the same thread.
		//	hmds->checkBeginFrameScope("ovrHmd_EndFrame");
		//	ThreadChecker::Scope checkScope(&hmds->RenderAPIThreadChecker, "ovrHmd_EndFrame");

		hmds->pRenderer->SetLatencyTestColor(hmds->LatencyTestActive ? hmds->LatencyTestDrawColor : NULL);

		ovrHmd_GetLatencyTest2DrawColor(hmddesc, NULL); // We don't actually need to draw color, so send NULL

		if (hmds->pRenderer)
		{
			hmds->pRenderer->SaveGraphicsState();

			// See if we need to show the HSWDisplay.
			if (hmds->pHSWDisplay) // Until we know that these are valid, assume any of them can't be.
			{
				ovrHSWDisplayState hswDisplayState;
				hmds->pHSWDisplay->TickState(&hswDisplayState);  // This may internally call HASWarning::Display.

				if (hswDisplayState.Displayed)
				{
					hmds->pHSWDisplay->Render(ovrEye_Left, &eyeTexture[ovrEye_Left]);
					hmds->pHSWDisplay->Render(ovrEye_Right, &eyeTexture[ovrEye_Right]);
				}
			}

			hmds->pRenderer->EndFrame(false);
			hmds->pRenderer->RestoreGraphicsState();
		}
		// Call after present
		ovrHmd_EndFrameTiming(hmddesc);

		// Out of BeginFrame
		hmds->BeginFrameThreadId = 0;
		hmds->BeginFrameCalled = false;
	}


	int fb_width, fb_height;

	RiftViewport::RiftViewport(User* user, OculusDeviceDriver* oculusDriver, Kernel* kernel) : Viewport(user)
	{
		this->eye = eye;
		this->oculusDriver = oculusDriver;

		ovrHmd hmd = *oculusDriver->hmd;

		/*	fbo = new FBO(640, 800, true);

			shader = new ShaderProgram("data/VrCave/shaders/rift.vert","data/VrCave/shaders/rift.frag");
			shader->bindAttributeLocation("a_position", 0);
			shader->bindAttributeLocation("a_texture", 1);
			shader->link();*/


		ovrGLConfig glcfg;

		ovrSizei sizeLeft = ovrHmd_GetFovTextureSize(*oculusDriver->hmd, ovrEye_Left, (*oculusDriver->hmd)->DefaultEyeFov[0], 1.0f);
		ovrSizei sizeRight = ovrHmd_GetFovTextureSize(*oculusDriver->hmd, ovrEye_Right, (*oculusDriver->hmd)->DefaultEyeFov[1], 1.0f);
		ovrSizei totalSize{ sizeLeft.w + sizeRight.w, glm::max(sizeLeft.h, sizeRight.h) };

		fb_width = totalSize.w;
		fb_height = totalSize.h;

		fbo = new gl::FBO(totalSize.w, totalSize.w, true);

		/* fill in the ovrGLTexture structures that describe our render target texture */
		for (int i = 0; i < 2; i++) {
			fb_ovr_tex[i].OGL.Header.API = ovrRenderAPI_OpenGL;
			fb_ovr_tex[i].OGL.Header.TextureSize.w = fbo->getWidth();
			fb_ovr_tex[i].OGL.Header.TextureSize.h = fbo->getHeight();
			/* this next field is the only one that differs between the two eyes */
			fb_ovr_tex[i].OGL.Header.RenderViewport.Pos.x = i == 0 ? 0 : (int)( totalSize.w / 2.0f );
			fb_ovr_tex[i].OGL.Header.RenderViewport.Pos.y = fbo->getHeight() - totalSize.h;
			fb_ovr_tex[i].OGL.Header.RenderViewport.Size.w = (int)(totalSize.w / 2.0);
			fb_ovr_tex[i].OGL.Header.RenderViewport.Size.h = totalSize.h;
			fb_ovr_tex[i].OGL.TexId = fbo->texid;	/* both eyes will use the same texture id */
		}
		memset(&glcfg, 0, sizeof glcfg);
		glcfg.OGL.Header.API = ovrRenderAPI_OpenGL;
		glcfg.OGL.Header.RTSize = hmd->Resolution;
		glcfg.OGL.Header.Multisample = 1;

		if (hmd->HmdCaps & ovrHmdCap_ExtendDesktop) {
			printf("running in \"extended desktop\" mode\n");
		}
		else {
			/* to sucessfully draw to the HMD display in "direct-hmd" mode, we have to
			* call ovrHmd_AttachToWindow
			* XXX: this doesn't work properly yet due to bugs in the oculus 0.4.1 sdk/driver
			*/
#ifdef WIN32
			HWND sys_win = GetActiveWindow();
			glcfg.OGL.Window = sys_win;
			glcfg.OGL.DC = wglGetCurrentDC();
			ovrHmd_AttachToWindow(hmd, sys_win, 0, 0);
#endif
			printf("running in \"direct-hmd\" mode\n");
		}
		/* enable low-persistence display and dynamic prediction for lattency compensation */
		ovrHmd_SetEnabledCaps(hmd, ovrHmdCap_LowPersistence | ovrHmdCap_DynamicPrediction);

		/* configure SDK-rendering and enable chromatic abberation correction, vignetting, and
		* timewrap, which shifts the image before drawing to counter any lattency between the call
		* to ovrHmd_GetEyePose and ovrHmd_EndFrame.
		*/
		unsigned int dcaps = ovrDistortionCap_Chromatic | ovrDistortionCap_Vignette | ovrDistortionCap_TimeWarp |
			ovrDistortionCap_Overdrive;
		if (!ovrHmd_ConfigureRendering(hmd, &glcfg.Config, dcaps, hmd->DefaultEyeFov, oculusDriver->eye_rdesc)) {
			fprintf(stderr, "failed to configure distortion renderer\n");
		}
		/* disable the retarded "health and safety warning" */
		//ovrhmd_EnableHSWDisplaySDKRender(hmd, 0);

		kernel->windowMoveTo(hmd->WindowsPos.x, hmd->WindowsPos.y);

	}

	glm::mat4 RiftViewport::getProjectionMatrix()
	{
		return glm::perspective(glm::radians(60.0f), 1.0f, 0.001f, 1000.0f);
	}


	float dist = 0;

	void RiftViewport::draw(Application* application)
	{
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);

		int i;
		ovrMatrix4f proj;
		ovrPosef pose[2];
		float rot_mat[16];


		/* start drawing onto our texture render target */
		fbo->bind();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/* for each eye ... */
		for (i = 0; i < 2; i++) {
			ovrEyeType eye = (*oculusDriver->hmd)->EyeRenderOrder[i];

			/* -- viewport transformation --
			* setup the viewport to draw in the left half of the framebuffer when we're
			* rendering the left eye's view (0, 0, width/2, height), and in the right half
			* of the framebuffer for the right eye's view (width/2, 0, width/2, height)
			*/
			glViewport(eye == ovrEye_Left ? 0 : fb_width / 2, 0, fb_width / 2, fb_height);

			/* -- projection transformation --
			* we'll just have to use the projection matrix supplied by the oculus SDK for this eye
			* note that libovr matrices are the transpose of what OpenGL expects, so we have to
			* use glLoadTransposeMatrixf instead of glLoadMatrixf to load it.
			*/
			proj = ovrMatrix4f_Projection((*oculusDriver->hmd)->DefaultEyeFov[eye], 0.05f, 700.0f, 1);
			glMatrixMode(GL_PROJECTION);
			glLoadTransposeMatrixf(proj.M[0]);

			/* -- view/camera transformation --
			* we need to construct a view matrix by combining all the information provided by the oculus
			* SDK, about the position and orientation of the user's head in the world.
			*/
			pose[eye] = ovrHmd_GetEyePose((*oculusDriver->hmd), eye);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(oculusDriver->eye_rdesc[eye].ViewAdjust.x, oculusDriver->eye_rdesc[eye].ViewAdjust.y, oculusDriver->eye_rdesc[eye].ViewAdjust.z);
			/* retrieve the orientation quaternion and convert it to a rotation matrix */
			quat_to_matrix(&pose[eye].Orientation.x, rot_mat);
			glMultMatrixf(rot_mat);
			/* translate the view matrix with the positional tracking */
			glTranslatef(-pose[eye].Position.x, -pose[eye].Position.y, -pose[eye].Position.z);
			/* move the camera to the eye level of the user */
			glTranslatef(0, -ovrHmd_GetFloat((*oculusDriver->hmd), OVR_KEY_EYE_HEIGHT, 1.65f) + 1.5f, 0);

			/* finally draw the scene for this eye */
			//		draw_scene();

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

			projectionMatrix = glm::transpose(projectionMatrix);

			glm::mat4 modelviewMatrix;
			modelviewMatrix = glm::translate(modelviewMatrix, glm::vec3(oculusDriver->eye_rdesc[eye].ViewAdjust.x, oculusDriver->eye_rdesc[eye].ViewAdjust.y, oculusDriver->eye_rdesc[eye].ViewAdjust.z));
			modelviewMatrix *= glm::make_mat4(rot_mat);
			modelviewMatrix = glm::translate(modelviewMatrix, glm::vec3(-pose[eye].Position.x, -pose[eye].Position.y, -pose[eye].Position.z));
			modelviewMatrix = glm::translate(modelviewMatrix, glm::vec3(0, -ovrHmd_GetFloat((*oculusDriver->hmd), OVR_KEY_EYE_HEIGHT, 1.65f) + 1.5f, 0));

			application->draw(projectionMatrix, modelviewMatrix);


		}

		/* after drawing both eyes into the texture render target, revert to drawing directly to the
		* display, and we call ovrHmd_EndFrame, to let the Oculus SDK draw both images properly
		* compensated for lens distortion and chromatic abberation onto the HMD screen.
		*/
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

		ovrHmd_EndFrame_noswap(*oculusDriver->hmd, pose, &fb_ovr_tex[0].Texture);
		//ovrHmd_EndFrame(*oculusDriver->hmd, pose, &fb_ovr_tex[0].Texture);

		assert(glGetError() == GL_NO_ERROR);


	}
}