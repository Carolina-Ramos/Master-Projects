
#include "Rocks.h"

#include <cmath>
# include <iostream>
#include <random>

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

#define ROCK_TIMER 10.f

Rocks::Rocks() : _posX(0.0f), _posZ(0.0f), _angle(0.0f), _speed(2.f), _alfa(0.0f), _timer(ROCK_TIMER) {
}

void Rocks::init() {
	MyMesh amesh;
	float sides, radius;

	float amb[] = { 0.2f, 0.15f, 0.1f, 1.0f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 100.0f;
	int texcount = 0;

	// create geometry and VAO of the sphere
	radius = 0.3f;
	amesh = createSphere(radius, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslationsRocks.push_back({0.0f, radius, 0});
	_modelRotationsRocks.push_back({ 0.0f, 0.0f, 1.0f, 0.0f });

	for (int i = 0; i < 4; i++) {
		_box.corners[i].y = 0;
	}
	for (int i = 4; i < 8; i++) {
		_box.corners[i].y = radius;
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

	createRock();
}

void Rocks::reset() {
	_speed = 2.f;
	_timer = ROCK_TIMER;
	createRock();
}

void Rocks::createRock() {
	_alfa = ((float)rand() / RAND_MAX) * 360.f;
	_posX = 40 * cos(_alfa * 3.14f / 180.0f);
	_posZ = 40 * sin(_alfa * 3.14f / 180.0f);
	float range = 120.f;
	_angle = (((float)rand() / (float)RAND_MAX) * range) - (range / 2) + (_alfa - 180.f);

	updateMinMax();
}

void Rocks::update(float deltaTime) {
	// Move position
	float speedVecX = _speed * cos(_angle * 3.14f / 180.0f);
	float speedVecZ = _speed * sin(_angle * 3.14f / 180.0f);
	_posX += speedVecX * deltaTime;
	_posZ += speedVecZ * deltaTime;

	// Respawn if far away
	if (sqrt(pow(_posX, 2) + pow(_posZ, 2)) > 40) {
		createRock();
	}

	// Increase speed with timer
	_timer -= deltaTime;
	if (_timer < 0) {
		_speed += 2.f;
		_timer = ROCK_TIMER;
	}

	updateMinMax();
}

float Rocks::getPositionX() {
	return _posX;
}

float Rocks::getPositionZ() {
	return _posZ;
}

void Rocks::render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId) {
	GLint loc;

	glUseProgram(shader);

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
		rotate(MODEL, _angle, 0.0f, 1.0f, 0.0f);

		// Preform mesh-specific transformations
		translate(MODEL, _modelTranslationsRocks[objId].x, _modelTranslationsRocks[objId].y, _modelTranslationsRocks[objId].z);
		rotate(MODEL, _modelRotationsRocks[objId].angle, _modelRotationsRocks[objId].x, _modelRotationsRocks[objId].y, _modelRotationsRocks[objId].z);

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

void Rocks::updateMinMax() {
	float minX, minY, minZ, maxX, maxY, maxZ;

	pushMatrix(MODEL);
	loadIdentity(MODEL);
	translate(MODEL, _posX, 0.0f, _posZ);
	rotate(MODEL, _angle, 0.0f, 1.0f, 0.0f);
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
		rotate(MODEL, _angle, 0.0f, 1.0f, 0.0f);
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

MinMaxBox Rocks::getMinMax() {
	return _minMax;
}
