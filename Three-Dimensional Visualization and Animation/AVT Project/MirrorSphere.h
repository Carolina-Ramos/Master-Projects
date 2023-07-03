#pragma once

#include <vector>
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "geometry.h"

struct TranslationMirrorSphere {
	float x;
	float y;
	float z;
};

class MirrorSphere {
public:
	MirrorSphere();
	void init();
	void render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId);
	void createMirrorSphere(float posX, float posZ);
private:
	std::vector<struct MyMesh> _modelMeshes;
	std::vector<struct TranslationMirrorSphere> _modelTranslations;
	float _posX, _posZ;
};

