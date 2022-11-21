#ifndef ROVER_H_
#define ROVER_H_

#include <vector>
#include <array>
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "geometry.h"
#include "Box.h"

struct Scale {
	float x;
	float y;
	float z;
};

struct Translation {
	float x;
	float y;
	float z;
};

struct Rotation {
	float angle;
	float x;
	float y;
	float z;
};

struct Speed {
	float speed;
	float x;
	float z;
};

struct RoverLights {
	std::array<float, 4> pos1;
	std::array<float, 4> pos2;
	std::array<float, 4> dir;
};

struct Collision {
	int id;
	float speed;
};

class Rover {
public:
	Rover();
	void init();
	void reset();
	Collision update(float deltaTime, std::vector<MinMaxBox> boxes);
	void render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId);
	void move(int dir);
	float getSpeed();
	std::array<float, 3> getFollowTarget();
	std::array<float, 3> getFollowPos(float alpha, float beta, float r);
	std::array<float, 4> getPos();
	RoverLights getRoverLights();
	void updateMinMax(float posX, float posZ, float angle);
private:
	std::vector<struct MyMesh> _modelMeshes;
	std::vector<struct Translation> _modelTranslations;
	std::vector<struct Rotation> _modelRotations;
	std::vector<struct Scale> _modelScales;
	float _posX, _posZ;
	float _angle, _wheelRotAngle, _wheelSpdAngle;
	Speed _speedVec;
	Box _box;
	MinMaxBox _minMax;
	int _moveState, _rotateState;
};

#endif

