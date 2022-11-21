
#ifndef ROCKS_H_
#define ROCKS_H_

#include <vector>
#include <iostream>
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "geometry.h"
#include "Box.h"

struct TranslationRocks {
	float x;
	float y;
	float z;
};

struct RotationRocks {
	float angle;
	float x;
	float y;
	float z;
};

class Rocks {
public:
	Rocks();
	void init();
	void reset();
	void update(float deltaTime);
	void render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId);
	void createRock();
	void updateMinMax();
	MinMaxBox getMinMax();
	float getPositionX();
	float getPositionZ();
private:
	std::vector<struct MyMesh> _modelMeshes;
	std::vector<struct TranslationRocks> _modelTranslationsRocks;
	std::vector<struct RotationRocks> _modelRotationsRocks;
	float _posX, _posZ, _alfa;
	float _angle;
	float _speed;
	float _timer;
	Box _box;
	MinMaxBox _minMax;
};

#endif