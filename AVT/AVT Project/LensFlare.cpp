#include "LensFlare.h"

inline int clampi(const int x, const int min, const int max) {
	return (x < min ? min : (x > max ? max : x));
}

LensFlare::LensFlare() : _elements(), _mesh(), _textureArray(), _scale(), _maxSize() {
	_lightPos = { 0.f, 0.f };
	_viewport = { 0, 0, 0, 0 };
}

void LensFlare::init() {
	glGenTextures(FLARE_TEX_COUNT, _textureArray);
	Texture2D_Loader(_textureArray, "sun.tga", 0);
	Texture2D_Loader(_textureArray, "flar.tga", 1);
	Texture2D_Loader(_textureArray, "ring.tga", 2);

	_elements.push_back(createElement(0.0f, 2.0f, {1.f, 1.f, 1.f, 1.f}, 0));
	_elements.push_back(createElement(0.1f, 0.2f, { 1.f, 1.f, 1.f, 1.f }, 1));
	_elements.push_back(createElement(0.4f, 0.2f, { 1.f, 1.f, 1.f, 1.f }, 2));
	_elements.push_back(createElement(0.8f, 0.1f, { 1.f, 1.f, 1.f, 1.f }, 1));
	_elements.push_back(createElement(1.0f, 0.1f, { 1.f, 1.f, 1.f, 1.f }, 2));

	// create geometry and VAO of the quad for flare elements
	_mesh = createQuad(1, 1);

	_scale = 1.0f;
	_maxSize = 0.2f;
}

void LensFlare::updateLight(std::array<float, 2> projectedLightPos, std::array<int, 4> viewport) {
	_lightPos = projectedLightPos;
	_viewport = viewport;
}

void LensFlare::render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId) {
	int     dx, dy;          // Screen coordinates of "destination"
	int     px, py;          // Screen coordinates of flare element
	int		cx, cy;
	float    maxflaredist, flaredist, flaremaxsize, flarescale, scaleDistance;
	int     width, height, alpha;    // Piece parameters;
	int     i;
	float	diffuse[4];

	GLint loc;

	int screenMaxCoordX = _viewport[0] + _viewport[2] - 1;
	int screenMaxCoordY = _viewport[1] + _viewport[3] - 1;

	if (_lightPos[0] < _viewport[0] || _lightPos[0] > screenMaxCoordX) return;
	if (_lightPos[1] < _viewport[1] || _lightPos[1] > screenMaxCoordY) return;

	//viewport center
	cx = _viewport[0] + (int)(0.5f * (float)_viewport[2]) - 1;
	cy = _viewport[1] + (int)(0.5f * (float)_viewport[3]) - 1;

	// Compute how far off-center the flare source is.
	maxflaredist = sqrt((double)cx * (double)cx + (double)cy * (double)cy);
	flaredist = sqrt((_lightPos[0] - (double)cx) * (_lightPos[0] - (double)cx) + (_lightPos[1] - (double)cy) * (_lightPos[1] - (double)cy));
	scaleDistance = (maxflaredist - flaredist) / maxflaredist;
	flaremaxsize = (int)(_viewport[2] * _maxSize);
	flarescale = (int)(_viewport[2] * _scale);

	// Destination is opposite side of centre from source
	dx = clampi(cx + (cx - _lightPos[0]), _viewport[0], screenMaxCoordX);
	dy = clampi(cy + (cy - _lightPos[1]), _viewport[1], screenMaxCoordY);

	for (int i = 0; i < _elements.size(); i++) {
		FlareElement element = _elements[i];

		// Position is interpolated along line between start and destination.
		px = (int)((1.0f - element.distance) * _lightPos[0] + element.distance * dx);
		py = (int)((1.0f - element.distance) * _lightPos[1] + element.distance * dy);
		px = clampi(px, _viewport[0], screenMaxCoordX);
		py = clampi(py, _viewport[1], screenMaxCoordY);

		// Piece size are 0 to 1; flare size is proportion of screen width; scale by flaredist/maxflaredist.
		width = (int)(scaleDistance * flarescale * element.scale);

		// Width gets clamped, to allows the off-axis flaresto keep a good size without letting the elements get big when centered.
		if (width > flaremaxsize)  width = flaremaxsize;

		height = (int)((float)_viewport[3] / (float)_viewport[2] * (float)width);
		memcpy(diffuse, element.color.data(), 4 * sizeof(float));
		diffuse[3] *= scaleDistance;   //scale the alpha channel

		if (width > 1) {
			// send the material - diffuse color modulated with texture
			loc = glGetUniformLocation(shader, "mat.diffuse");
			glUniform4fv(loc, 1, diffuse);

			glBindTexture(GL_TEXTURE_2D, _textureArray[element.texture]);
			pushMatrix(MODEL);
			translate(MODEL, (float)(px - width * 0.0f), (float)(py - height * 0.0f), 0.0f);
			scale(MODEL, (float)width, (float)height, 1);
			computeDerivedMatrix(PROJ_VIEW_MODEL);
			glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
			glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
			computeNormalMatrix3x3();
			glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

			glBindVertexArray(_mesh.vao);
			glDrawElements(_mesh.type, _mesh.numIndexes, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);
			popMatrix(MODEL);
		}
	}
}

FlareElement LensFlare::createElement(float distance, float scale, std::array<float, 4> color, int textureIndex) {
	FlareElement fle;

	fle.distance = distance;
	fle.scale = scale;
	fle.color = color;
	fle.texture = textureIndex;

	return fle;
}
