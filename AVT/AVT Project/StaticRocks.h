#ifndef STATICROCKS_H_
#define STATICROCKS_H_

#include <vector>
#include <array>
#include <cmath>
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "geometry.h"
#include "Box.h"

struct TranslationStatic {
	float x;
	float y;
	float z;
};

struct PushSpeed {
	float speed;
	float x;
	float z;
};

class StaticRocks {
public:
	StaticRocks();
	void init();
	void reset();
	void update(float deltaTime);
	void render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId);
	void createStaticRocks(float posX, float posZ);
	void pushFromPos(std::array<float, 4> pos, float speed);
	void updateMinMax();
	MinMaxBox getMinMax();
private:
	std::vector<struct MyMesh> _modelMeshes;
	std::vector<struct TranslationStatic> _modelTranslationStaticRocks;
	float _posX, _posZ;
	PushSpeed _speedVec;
	Box _box;
	MinMaxBox _minMax;
};

#endif