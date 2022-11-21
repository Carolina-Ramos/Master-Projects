#include "Camera.h"

Camera::Camera() : _type(CAM_PERSPECTIVE) {
}

void Camera::init(CameraType type, float pos[3], float target[3], bool topDown) {
	_type = type;
	_pos[0] = pos[0]; _pos[1] = pos[1]; _pos[2] = pos[2];
	_target[0] = target[0]; _target[1] = target[1]; _target[2] = target[2];
	
	if (topDown) {
		_up[1] = 0.0f;
		_up[0] = 1.0f;
	}
}

void Camera::updatePos(std::array<float, 3> pos) {
	_pos[0] = pos[0]; _pos[1] = pos[1]; _pos[2] = pos[2];
}

void Camera::updateTarget(std::array<float, 3> target) {
	_target[0] = target[0]; _target[1] = target[1]; _target[2] = target[2];
}

void Camera::camLookAt() {
	lookAt(_pos[0], _pos[1], _pos[2], _target[0], _target[1], _target[2], _up[0], _up[1], _up[2]);
}

void Camera::camProjection(int viewport[4]) {
	float ratio = (1.0f * viewport[2]) / viewport[3];

	switch (_type) {
	case CAM_PERSPECTIVE:
		perspective(53.13f, ratio, 0.1f, 1000.0f);
		break;
	case CAM_ORTHO:
		ortho(-40.f * ratio, 40.f * ratio, -40.f, 40.f, 0.1f, 1000.0f);
		break;
	default:
		break;
	}
}

std::array<float, 4> Camera::getPos() {
	//return { 0.f, 0.f, 0.f, 1.f };
	return { _pos[0], _pos[1], _pos[2], 1.f };
}
