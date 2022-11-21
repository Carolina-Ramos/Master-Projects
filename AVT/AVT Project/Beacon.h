#ifndef BEACON_H_
#define BEACON_H_

#include <vector>
#include <array>
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "geometry.h"
#include "Box.h"

struct ScaleBeacon {
	float x;
	float y;
	float z;
};

struct TranslationBeacon {
	float x;
	float y;
	float z;
};

struct RotationBeacon {
	float angle;
	float x;
	float y;
	float z;
};

class Beacon {
public:
	Beacon();
	void init();
	void render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId);
	void Beacon::createBeacon(float posX, float posZ);
	void updateMinMax();
	MinMaxBox getMinMax();
private:
	std::vector<struct MyMesh> _modelMeshes;
	std::vector<struct TranslationBeacon> _modelTranslations;
	std::vector<struct RotationBeacon> _modelRotations;
	std::vector<struct ScaleBeacon> _modelScales;
	float _posX, _posZ;
	Box _box;
	MinMaxBox _minMax;
};

#endif