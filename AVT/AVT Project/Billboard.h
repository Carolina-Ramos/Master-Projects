#ifndef BILLBOARD_H_
#define BILLBOARD_H_

#include <vector>
#include <array>
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "geometry.h"
#include "Box.h"

struct ScaleBillboard {
	float x;
	float y;
	float z;
};

struct TranslationBillboard {
	float x;
	float y;
	float z;
};

struct RotationBillboard {
	float angle;
	float x;
	float y;
	float z;
};

class Billboard {
public:
	Billboard();
	void init();
	void reset();
	void render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId, bool reflect);
private:
	std::vector<struct MyMesh> _modelMeshes;
	std::vector<struct TranslationBillboard> _modelTranslationsBillboard;
	std::vector<struct RotationBillboard> _modelRotationsBillboard;
	std::vector<struct ScaleBillboard> _modelScalesBillboard;
	float _posX, _posZ;
	float _angle;
	int _moveState, _rotateState;
};

#endif