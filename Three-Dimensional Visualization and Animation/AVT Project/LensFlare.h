#pragma once

#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <GL/glew.h>
#include "AVTmathLib.h"
#include "Texture_Loader.h"
#include "geometry.h"

#define FLARE_TEX_COUNT 3

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];
/// The normal matrix
extern float mNormal3x3[9];

struct FlareElement {
	float distance;
	float scale;
	std::array<float, 4> color;
	int texture;
};

class LensFlare {
public:
	LensFlare();
	void init();
	void render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId);
	void updateLight(std::array<float, 2> projectedLightPos, std::array<int, 4> viewport);
private:
	std::vector<FlareElement> _elements;
	std::array<float, 2> _lightPos;
	std::array<int, 4> _viewport;
	MyMesh _mesh;
	GLuint _textureArray[FLARE_TEX_COUNT];
	float  _scale;     // Scale factor for adjusting overall size of flare elements.
	float  _maxSize;   // Max size of largest element, as proportion of screen width (0.0-1.0)
	FlareElement createElement(float distance, float scale, std::array<float, 4> color, int textureIndex);
};

