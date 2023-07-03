
#ifndef OBJECTTRANSP_H_
#define OBJECTTRANSP_H_

#include <vector>
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "geometry.h"
#include "Box.h"

struct TranslationObject {
	float x;
	float y;
	float z;
};

class ObjectTransp {
public:
	ObjectTransp();
	void init();
	void render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId);
	void createObjectTransp(float posX, float posZ);
	void updateMinMax();
	MinMaxBox getMinMax();
private:
	std::vector<struct MyMesh> _modelMeshes;
	std::vector<struct TranslationObject> _modelTranslationObject;
	float _posX, _posZ;
	Box _box;
	MinMaxBox _minMax;
};

#endif