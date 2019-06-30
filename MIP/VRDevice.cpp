/*#include "stdAfx.h"
#include "../VolumeRendering_cp1/VRDevice.h"





void clearBuffer(int width, int height)
{
	// Background Fill Color
	glViewport(0, 0, width, height);
	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void VRDevice::initSystem()
{
	vr::HmdError init_error = vr::VRInitError_None;
	if (!vr::VR_IsHmdPresent())
	{
		std::cerr << "VR HMD is not presented" << std::endl;
	}

	if (!vr::VR_IsRuntimeInstalled())
	{
		std::cerr << "OpenVR runtime is not installed" << std::endl;
	}

	vr_system = vr::VR_Init(&init_error, vr::VRApplication_Scene);

	if (init_error != vr::VRInitError_None)
	{
		std::cerr << "Error upon initializing VR" << std::endl;
		std::cerr << "Check Steam for Error message" << std::endl;
	}

	if (vr_system)
	{
		vr_system->GetRecommendedRenderTargetSize(&width, &height);
		//width = 1280;
		//height = 800;
		vr::VRCompositor();
	}
}

void VRDevice::setupFBO(TextureRender &eye)
{
	if (vr_system)
	{
		glGenFramebuffers(1, &eye.fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, eye.fbo);

		glGenRenderbuffers(1, &eye.rbo);
		glBindRenderbuffer(GL_RENDERBUFFER, eye.rbo);
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, width, height);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, eye.rbo);

		glGenTextures(1, &eye.texture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, eye.texture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, width, height, true);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, eye.texture, 0);

		glGenFramebuffers(1, &eye.final_fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, eye.final_fbo);

		glGenTextures(1, &eye.final_texture);
		glBindTexture(GL_TEXTURE_2D, eye.final_texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, eye.final_texture, 0);

		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			std::cerr << "Something wrong with creating fbo" << std::endl;
			std::cerr << status << std::endl;
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void VRDevice::activateLeft()
{
	if (vr_system)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, left_eye.fbo);
		clearBuffer(width, height);
	}
}

void VRDevice::activateRight()
{
	if (vr_system)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, right_eye.fbo);
		clearBuffer(width, height);
	}
}

void VRDevice::renderLeft()
{
	if (vr_system)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, left_eye.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, left_eye.final_fbo);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBindTexture(GL_TEXTURE_2D, left_eye.final_texture);
		// screencapture(width, height);
		vr::Texture_t leftEyeTexture = { (void *)(uintptr_t)left_eye.final_texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Left, &leftEyeTexture);
	}
}

void VRDevice::renderRight()
{
	if (vr_system)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, right_eye.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, right_eye.final_fbo);
		glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, right_eye.fbo);
		//screencapture(width, height);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		vr::Texture_t rightEyeTexture = { (void *)(uintptr_t)right_eye.final_texture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
		vr::VRCompositor()->Submit(vr::Eye_Right, &rightEyeTexture);
	}
}

VRDevice::VRDevice()
{
	vr_system = nullptr;
	initSystem();
	if (vr_system)
	{
		setupFBO(left_eye);
		setupFBO(right_eye);
	}
	std::cout << "the width" << width;
}

VRDevice::~VRDevice()
{
	if (vr_system)
	{
		vr::VR_Shutdown();
	}
}

VRDevice &VRDevice::getInstance()
{
	static VRDevice instance;
	return instance;
}

void VRDevice::updateHMDPose()
{
	if (!vr_system)
		return;

	vr::VRCompositor()->WaitGetPoses(device_poses, vr::k_unMaxTrackedDeviceCount, NULL, 0);

	for (int i = 0; i < vr::k_unMaxTrackedDeviceCount; i++)
	{
		if (device_poses[i].bPoseIsValid)
		{
			device_poses_mtx[i] = glm::inverse(glm::mat4(toGLMtx34(device_poses[i].mDeviceToAbsoluteTracking)));
		}
	}

}

void VRDevice::setupEyeMtx()
{
	l_eye_mtx = glm::mat4(0);
	r_eye_mtx = glm::mat4(0);

	if (!vr_system)
		return;
	l_eye_mtx = glm::inverse(glm::mat4(toGLMtx34(vr_system->GetEyeToHeadTransform(vr::Eye_Left))));
	r_eye_mtx = glm::inverse(glm::mat4(toGLMtx34(vr_system->GetEyeToHeadTransform(vr::Eye_Right))));

}

void VRDevice::setupProjMtx(float nearPlane, float farPlane)
{
	l_proj_mtx = glm::mat4(0);
	r_proj_mtx = glm::mat4(0);
	if (!vr_system) return;

	l_proj_mtx = toGLMtx4(vr_system->GetProjectionMatrix(vr::Eye_Left, nearPlane, farPlane));
	r_proj_mtx = toGLMtx4(vr_system->GetProjectionMatrix(vr::Eye_Right, nearPlane, farPlane));
}*/
