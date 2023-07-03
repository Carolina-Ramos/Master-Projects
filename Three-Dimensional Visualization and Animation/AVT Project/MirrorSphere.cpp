#include "MirrorSphere.h"

#include <cmath>
# include <iostream>
#include <random>

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

MirrorSphere::MirrorSphere() : _posX(0.0f), _posZ(0.0f) {
}

void MirrorSphere::init() {
	MyMesh amesh;
	float y_distance = 0.8f;
	float radius = 5;

	float amb[] = { 0.0f, 0.1f, 0.2f, 1.0f };
	float diff[] = { 0.4f, 0.6f, 0.8f, 1.0f };
	float spec[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 200.0f;
	int texcount = 0;

	// create geometry and VAO of the sphere
	amesh = createSphere(2.0f, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslations.push_back({ 0.0f, y_distance * 3, 0.0f });
}

void MirrorSphere::createMirrorSphere(float posX, float posZ) {
	_posX = posX;
	_posZ = posZ;
}

void MirrorSphere::render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId) {
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
		translate(MODEL, _modelTranslations[objId].x, _modelTranslations[objId].y, _modelTranslations[objId].z);

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
