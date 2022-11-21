
#include "StaticRocks.h"

#include <cmath>
# include <iostream>
#include <random>

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

StaticRocks::StaticRocks() : _posX(0.0f), _posZ(0.0f), _speedVec({ 0.0, 0.0, 0.0}) {
}

void StaticRocks::init() {
	MyMesh amesh;
	float sides, radius;
	float y_distance = 0.8f;

	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texcount = 0;

	// create geometry and VAO of the sphere
	radius = 1.0f;
	amesh = createCone( 1, radius, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslationStaticRocks.push_back({ 0.0f, 0.0f, 0 });

	for (int i = 0; i < 4; i++) {
		_box.corners[i].y = 0;
	}
	for (int i = 4; i < 8; i++) {
		_box.corners[i].y = radius*2;
	}

	_box.corners[0].x = -radius;
	_box.corners[3].x = -radius;
	_box.corners[4].x = -radius;
	_box.corners[7].x = -radius;

	_box.corners[1].x = radius;
	_box.corners[2].x = radius;
	_box.corners[5].x = radius;
	_box.corners[6].x = radius;

	_box.corners[0].z = -radius;
	_box.corners[1].z = -radius;
	_box.corners[4].z = -radius;
	_box.corners[5].z = -radius;

	_box.corners[2].z = radius;
	_box.corners[3].z = radius;
	_box.corners[6].z = radius;
	_box.corners[7].z = radius;

}

void StaticRocks::reset() {
	_speedVec = { 0.0, 0.0, 0.0 };
}

void StaticRocks::update(float deltaTime) {
	float slowDown = 10;
	if (_speedVec.speed > 0) {
		_speedVec.speed -= slowDown * deltaTime;
		if (_speedVec.speed < 0) _speedVec.speed = 0;

		_posX += _speedVec.speed * _speedVec.x * deltaTime;
		_posZ += _speedVec.speed * _speedVec.z * deltaTime;
	}
	updateMinMax();
}

void StaticRocks::render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId) {
	GLint loc;

	glUseProgram(shader);

	float y_distance = 0.0f;

	for (int objId = 0; objId < _modelMeshes.size(); objId++) {
		// send the material
		loc = glGetUniformLocation(shader, "mat.ambient");
		glUniform4fv(loc, 1, _modelMeshes[objId].mat.ambient);
		loc = glGetUniformLocation(shader, "mat.diffuse");
		glUniform4fv(loc, 1, _modelMeshes[objId].mat.diffuse);
		loc = glGetUniformLocation(shader, "mat.specular");
		glUniform4fv(loc, 1, _modelMeshes[objId].mat.specular);
		loc = glGetUniformLocation(shader, "mat.shininess");
		glUniform1f(loc, _modelMeshes[objId].mat.shininess);
		pushMatrix(MODEL);

		// Preform full model transformations
		translate(MODEL, _posX, 0.0f, _posZ);

		// Preform mesh-specific transformations
		translate(MODEL, _modelTranslationStaticRocks[objId].x, _modelTranslationStaticRocks[objId].y, _modelTranslationStaticRocks[objId].z);

		// send matrices to OGL
		computeDerivedMatrix(PROJ_VIEW_MODEL);
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(_modelMeshes[objId].vao);

		glDrawElements(_modelMeshes[objId].type, _modelMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
	}
}

void StaticRocks::createStaticRocks(float posX, float posZ) {
	_posX = posX;
	_posZ = posZ;
	updateMinMax();
}

void StaticRocks::pushFromPos(std::array<float, 4> pos, float speed) {
	float vecX = _posX - pos[0];
	float vecZ = _posZ - pos[2];
	float length = sqrt(vecX * vecX + vecZ * vecZ);

	_speedVec.x = vecX / length;
	_speedVec.z = vecZ / length;
	_speedVec.speed = abs(speed) / 2;
	if (_speedVec.speed < 0.1) _speedVec.speed = 0;
}

void StaticRocks::updateMinMax() {
	float minX, minY, minZ, maxX, maxY, maxZ;

	pushMatrix(MODEL);
	loadIdentity(MODEL);
	translate(MODEL, _posX, 0.0f, _posZ);
	float min[4] = { _box.corners[0].x, _box.corners[0].y, _box.corners[0].z, 1 };
	float res[4];
	multMatrixPoint(MODEL, min, res);
	popMatrix(MODEL);

	minX = res[0];
	minY = res[1];
	minZ = res[2];
	maxX = res[0];
	maxY = res[1];
	maxZ = res[2];
	for (int i = 1; i < 8; i++) {
		pushMatrix(MODEL);
		loadIdentity(MODEL);
		translate(MODEL, _posX, 0.0f, _posZ);
		float min[4] = { _box.corners[i].x, _box.corners[i].y, _box.corners[i].z, 1 };
		float res[4];
		multMatrixPoint(MODEL, min, res);
		if (res[0] < minX) minX = res[0];
		if (res[1] < minY) minY = res[1];
		if (res[2] < minZ) minZ = res[2];
		if (res[0] > maxX) maxX = res[0];
		if (res[1] > maxY) maxY = res[1];
		if (res[2] > maxZ) maxZ = res[2];
		popMatrix(MODEL);
	}
	_minMax.min.x = minX;
	_minMax.min.y = minY;
	_minMax.min.z = minZ;
	_minMax.max.x = maxX;
	_minMax.max.y = maxY;
	_minMax.max.z = maxZ;
}

MinMaxBox StaticRocks::getMinMax() {
	return _minMax;
}
