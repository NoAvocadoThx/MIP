#ifndef __MTXADAPTOR_H__
#define __MTXADAPTOR_H__


#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <openvr.h>


glm::mat4 toGLMtx4(vr::HmdMatrix44_t vrMtx);
glm::mat3x4 toGLMtx34(vr::HmdMatrix34_t vrMtx);

#endif // __MTXADAPTOR_H__

