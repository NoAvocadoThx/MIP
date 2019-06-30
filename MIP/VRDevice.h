#ifndef __VRDEVICE_H__
#define __VRDEVICE_H__
// System Headers

#include <glad/glad.h>

#include <GLFW/glfw3.h>


// GLM
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

// Standard Headers
typedef unsigned int uint;
//IO
#include <cstdio>
#include <iostream>

//STL
#include <vector>

//MISC
#include <cstdlib>
//#include <stb_image.h>
//#include <stb_image_write.h>
#include "MtxAdaptor.h"
#include <openvr.h>
class VRDevice
{

public:
	struct TextureRender
	{
		unsigned int fbo;
		unsigned int texture;
		unsigned int rbo;
		unsigned int final_fbo;
		unsigned int final_texture;
	};

	// Rendering
	TextureRender left_eye;
	TextureRender right_eye;
	// OpenVR System
	vr::IVRSystem *vr_system = nullptr;
	// Head Monitor Width and Height
	uint width;
	uint height;

	~VRDevice();
	static VRDevice &getInstance();


	VRDevice(VRDevice const &) = delete;
	void operator=(VRDevice const &) = delete;

	void activateLeft();
	void activateRight();

	void renderLeft();
	void renderRight();

	void setupProjMtx(float nearPlane, float farPlane);
private:
	VRDevice();
	void initSystem();
	void setupFBO(TextureRender&);
	void setupEyeMtx();



	void updateHMDPose();


	


	// Device Pose
	vr::TrackedDevicePose_t device_poses[vr::k_unMaxTrackedDeviceCount];
	glm::mat4 device_poses_mtx[vr::k_unMaxTrackedDeviceCount];

	glm::mat4 l_eye_mtx;
	glm::mat4 r_eye_mtx;

	glm::mat4 l_proj_mtx;
	glm::mat4 r_proj_mtx;

};



#endif // __VRDEVICE_H__



