#include "stdAfx.h"
#include "MtxAdaptor.h"



glm::mat4 toGLMtx4(vr::HmdMatrix44_t vrMtx) {
	glm::mat4 mtx(0);

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			mtx[i][j] = vrMtx.m[i][j];
		}
	}
	return glm::transpose(mtx);
}

glm::mat3x4 toGLMtx34(vr::HmdMatrix34_t vrMtx) {
	glm::mat3x4 mtx(0);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 4; j++) {
			mtx[i][j] = vrMtx.m[i][j];
		}
	}
	return glm::transpose(mtx);
}