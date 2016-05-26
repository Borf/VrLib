#define NOMINMAX

#include "OpenVRViewport.h"

#include <VrLib/Application.h>
#include <VrLib/User.h>
#include <VrLib/drivers/Oculus.h>
#include <VrLib/Kernel.h>

#include <VrLib/gl/FBO.h>
#include <VrLib/gl/Shader.h>
#include <VrLib/Log.h>
#include <VrLib/drivers/OpenVR.h>

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/wglew.h>
#include <openvr.h>

namespace vrlib
{

	OpenVRViewport::OpenVRViewport(User* user, OpenVRDriver* driver, Kernel* kernel) : Viewport(user)
	{
		this->openVRDriver = driver;
		m_pHMD = driver->m_pHMD;
		if (!m_pHMD)
			return;

		m_pHMD->GetRecommendedRenderTargetSize(&m_nRenderWidth, &m_nRenderHeight);

		CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, leftEyeDesc);
		CreateFrameBuffer(m_nRenderWidth, m_nRenderHeight, rightEyeDesc);

		vr::HmdMatrix44_t mat = m_pHMD->GetProjectionMatrix(vr::Eye_Left, 0.1f, 200.0f, vr::API_OpenGL);
		projectionLeft = glm::transpose(glm::make_mat4((float*)mat.m));

		mat = m_pHMD->GetProjectionMatrix(vr::Eye_Right, 0.1f, 200.0f, vr::API_OpenGL);
		projectionRight = glm::transpose(glm::make_mat4((float*)mat.m));


		vr::HmdMatrix34_t matEye = m_pHMD->GetEyeToHeadTransform(vr::Eye_Left);
		glm::mat4 matEyeLeft(glm::transpose(glm::mat4(glm::make_mat3x4((float*)matEye.m))));
		matEye = m_pHMD->GetEyeToHeadTransform(vr::Eye_Right);
		glm::mat4 matEyeRight(glm::transpose(glm::mat4(glm::make_mat3x4((float*)matEye.m))));


		viewLeft = glm::inverse(matEyeLeft);
		viewRight = glm::inverse(matEyeRight);
	}

	glm::mat4 OpenVRViewport::getProjectionMatrix()
	{
		return glm::perspective(glm::radians(60.0f), 1.0f, 0.001f, 1000.0f);
	}


	void OpenVRViewport::draw(Application* application)
	{
		if (!m_pHMD)
			return;
		GLint viewport[4];
		glGetIntegerv(GL_VIEWPORT, viewport);


		glm::mat4 hmdMatrix = glm::inverse(openVRDriver->hmd);
		resetOpenGL();

		glEnable(GL_MULTISAMPLE);
		glBindFramebuffer(GL_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
		glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		application->draw(projectionLeft, viewLeft * hmdMatrix, glm::mat4());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_MULTISAMPLE);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nRenderFramebufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, leftEyeDesc.m_nResolveFramebufferId);

		glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glEnable(GL_MULTISAMPLE);
		resetOpenGL();

		// Right Eye
		glBindFramebuffer(GL_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
		glViewport(0, 0, m_nRenderWidth, m_nRenderHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		application->draw(projectionRight, viewRight * hmdMatrix, glm::mat4());
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		glDisable(GL_MULTISAMPLE);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, rightEyeDesc.m_nRenderFramebufferId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rightEyeDesc.m_nResolveFramebufferId);

		glBlitFramebuffer(0, 0, m_nRenderWidth, m_nRenderHeight, 0, 0, m_nRenderWidth, m_nRenderHeight,
			GL_COLOR_BUFFER_BIT,
			GL_LINEAR);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

/*
glDisable(GL_DEPTH_TEST);
glViewport( 0, 0, m_nWindowWidth, m_nWindowHeight );

glBindVertexArray( m_unLensVAO );
glUseProgram( m_unLensProgramID );

//render left lens (first half of index array )
glBindTexture(GL_TEXTURE_2D, leftEyeDesc.m_nResolveTextureId );
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
glDrawElements( GL_TRIANGLES, m_uiIndexSize/2, GL_UNSIGNED_SHORT, 0 );

//render right lens (second half of index array )
glBindTexture(GL_TEXTURE_2D, rightEyeDesc.m_nResolveTextureId  );
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
glDrawElements( GL_TRIANGLES, m_uiIndexSize/2, GL_UNSIGNED_SHORT, (const void *)(m_uiIndexSize) );

glBindVertexArray( 0 );
glUseProgram( 0 );
*/

		glBindFramebuffer(GL_READ_FRAMEBUFFER, leftEyeDesc.m_nResolveTextureId);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		GLint w = m_nRenderWidth;
		GLint h = m_nRenderHeight;
		glBlitFramebuffer(0, 0, w, h,
			viewport[0], viewport[1], viewport[2], viewport[3],
			GL_COLOR_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);


		vr::Texture_t leftEyeTexture = { (void*)leftEyeDesc.m_nResolveTextureId, vr::API_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
		vr::Texture_t rightEyeTexture = { (void*)rightEyeDesc.m_nResolveTextureId, vr::API_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);

	}






	bool OpenVRViewport::CreateFrameBuffer(int nWidth, int nHeight, FramebufferDesc &framebufferDesc)
	{
		glGenFramebuffers(1, &framebufferDesc.m_nRenderFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nRenderFramebufferId);

		glGenRenderbuffers(1, &framebufferDesc.m_nDepthBufferId);
		glBindRenderbuffer(GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, nWidth, nHeight);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, framebufferDesc.m_nDepthBufferId);

		glGenTextures(1, &framebufferDesc.m_nRenderTextureId);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, nWidth, nHeight, true);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, framebufferDesc.m_nRenderTextureId, 0);

		glGenFramebuffers(1, &framebufferDesc.m_nResolveFramebufferId);
		glBindFramebuffer(GL_FRAMEBUFFER, framebufferDesc.m_nResolveFramebufferId);

		glGenTextures(1, &framebufferDesc.m_nResolveTextureId);
		glBindTexture(GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, nWidth, nHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferDesc.m_nResolveTextureId, 0);

		// check FBO status
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			return false;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		return true;
	}

}