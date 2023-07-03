#pragma once

#include <array>
#include "AVTmathLib.h"

enum CameraType {
	CAM_PERSPECTIVE,
	CAM_ORTHO
};

class Camera {
public:
	Camera();
	void init(CameraType type, float pos[3], float target[3], bool topDown = false);
	void updatePos(std::array<float, 3> pos);
	void updateTarget(std::array<float, 3> target);
	void camLookAt();
	void camProjection(int viewport[4]);
	std::array<float, 4> getPos();
private:
	CameraType _type;
	float _pos[3] = { 0.0f, 0.0f, 0.0f };
	float _target[3] = { 0.0f, 0.0f, 0.0f };
	float _up[3] = { 0.0f, 1.0f, 0.0f };
};

