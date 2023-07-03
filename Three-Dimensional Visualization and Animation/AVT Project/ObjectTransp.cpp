
#include "ObjectTransp.h"

#include <cmath>
# include <iostream>
#include <random>

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

ObjectTransp::ObjectTransp() : _posX(0.0f), _posZ(0.0f) {
}

void ObjectTransp::init() {
	MyMesh amesh;
	float y_distance = 0.8f;
	float radius=5;

	float amb[] = { 0.2f, 0.1f, 0.0f, 0.5f };
	float diff[] = { 0.8f, 0.6f, 0.4f, 0.5f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 50.0f;
	int texcount = 0;

	for (int i = 0; i < 4; i++) {
		_box.corners[i].y = 0;
	}
	for (int i = 5; i < 8; i++) {
		_box.corners[i].y = radius;
	}

	// create geometry and VAO of the sphere
	float* p = circularProfile(0.0f, 3.14159f / 2.0f, radius, 50);
	amesh = computeVAO(50 + 1, p + 2, p, 50 * 2, 0.0f);
	
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslationObject.push_back({ 0.0f, 0.0f, 0 });

	_box.corners[0].x = - radius;
	_box.corners[3].x = -radius;
	_box.corners[4].x = -radius;
	_box.corners[7].x = -radius;

	_box.corners[1].x = radius;
	_box.corners[2].x = radius;
	_box.corners[5].x = radius;
	_box.corners[6].x = radius;

	_box.corners[0].z = - radius;
	_box.corners[1].z = -radius;
	_box.corners[4].z = -radius;
	_box.corners[5].z = -radius;

	_box.corners[2].z = radius;
	_box.corners[3].z = radius;
	_box.corners[6].z = radius;
	_box.corners[7].z = radius;


}

void ObjectTransp::createObjectTransp(float posX, float posZ) {
	_posX = posX;
	_posZ = posZ;

	updateMinMax();
}

void ObjectTransp::render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId) {
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
		translate(MODEL, _modelTranslationObject[objId].x, _modelTranslationObject[objId].y, _modelTranslationObject[objId].z);

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

void ObjectTransp::updateMinMax() {
	float minX, minY, minZ, maxX, maxY, maxZ;

	loadIdentity(MODEL);
	pushMatrix(MODEL);
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

MinMaxBox ObjectTransp::getMinMax() {
	return _minMax;
}
