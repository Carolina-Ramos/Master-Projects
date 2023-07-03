#include "Billboard.h"

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

Billboard::Billboard() : _posX(5.0f), _posZ(-5.0f), _angle(45.0f), _moveState(0), _rotateState(0) {
}

void Billboard::init() {
	MyMesh amesh;
	float sides;
	float y_distance = 0.8f;

	float amb[] = { 0.5f, 0.5f, 0.5f, 0.5f };
	float diff[] = { 0.8f, 0.1f, 0.1f, 1.0f };
	float spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 500.0f;
	int texcount = 0;

	// create geometry and VAO of the Billboard body
	//objId = 0
	sides = 1.5f;
	amesh = createQuad(5, 5);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslationsBillboard.push_back({ -20.0f, 2.5f, -20.0f });

	//objId = 1
	sides = 1.5f;
	amesh = createQuad(5, 5);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslationsBillboard.push_back({ 20.0f, 2.5f, -30.0f });

	//objId = 2
	sides = 1.5f;
	amesh = createQuad(5, 5);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslationsBillboard.push_back({ 27.0f, 2.5f, -35.0f });

	//objId = 3
	sides = 1.5f;
	amesh = createQuad(5, 5);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslationsBillboard.push_back({ 35.0f, 2.5f, -40.0f });

	//objId = 4
	sides = 1.5f;
	amesh = createQuad(5, 5);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslationsBillboard.push_back({ 30.0f, 2.5f, -30.0f });
}

void Billboard::reset() {
	_posX = 2.0f;
	_posZ = 2.0f;
	_angle = 45.f;
	_moveState = 0;
	_rotateState = 0;
}

void Billboard::render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId, bool reflect) {
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
		pushMatrix(VIEW);

		// Preform full model transformations
		translate(MODEL, _posX, -1.f, _posZ);

		// Preform mesh-specific transformations
		translate(MODEL, _modelTranslationsBillboard[objId].x, _modelTranslationsBillboard[objId].y, _modelTranslationsBillboard[objId].z);

		// Preform billboard viewModel manipulation
		computeDerivedMatrix(VIEW_MODEL);
		float viewModel[16];
		memcpy(viewModel, mCompMatrix[VIEW_MODEL], sizeof(float) * 16);
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (i == j)
					viewModel[i * 4 + j] = 1.0;
				else
					viewModel[i * 4 + j] = 0.0;
			}
		}
		if (reflect) viewModel[1 * 4 + 1] = -1;
		memcpy(mCompMatrix[VIEW_MODEL], viewModel, sizeof(float) * 16);

		// send matrices to OGL
		computeDerivedMatrix_PVM();
		glUniformMatrix4fv(vm_uniformId, 1, GL_FALSE, mCompMatrix[VIEW_MODEL]);
		glUniformMatrix4fv(pvm_uniformId, 1, GL_FALSE, mCompMatrix[PROJ_VIEW_MODEL]);
		computeNormalMatrix3x3();
		glUniformMatrix3fv(normal_uniformId, 1, GL_FALSE, mNormal3x3);

		// Render mesh
		glBindVertexArray(_modelMeshes[objId].vao);

		glDrawElements(_modelMeshes[objId].type, _modelMeshes[objId].numIndexes, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		popMatrix(MODEL);
		popMatrix(VIEW);
	}
}