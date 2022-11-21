#include "Beacon.h"

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

Beacon::Beacon() : _posX(0), _posZ(0) {
}

void Beacon::init() {
	MyMesh amesh;
	float post_radius = 0.1f, height = 5.0f;
	float y_distance = height / 2;
	float bulb_radius = 0.5f;

	for (int i = 0; i < 4; i++) {
		_box.corners[i].y = 0;
	}

	float amb[] = { 0.1f, 0.0f, 0.3f, 1.0f };
	float diff[] = { 0.1f, 0.1f, 0.8f, 1.0f };
	float spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 500.0f;
	int texcount = 0;

	float amb1[] = { 0.3f, 0.3f, 0.3f, 1.0f };
	float diff1[] = { 0.1f, 0.1f, 0.8f, 1.0f };

	float amb2[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float diff2[] = { 0.1f, 0.1f, 0.8f, 0.5f };

	// create geometry and VAO of the beacon post
	amesh = createCylinder(height, post_radius, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslations.push_back({ 0, y_distance, 0 });
	_modelRotations.push_back({ 0.0f, 0.0f, 1.0f, 0.0f });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	_box.corners[0].x = -post_radius;
	_box.corners[3].x = -post_radius;
	_box.corners[4].x = -post_radius;
	_box.corners[7].x = -post_radius;

	_box.corners[1].x = post_radius;
	_box.corners[2].x = post_radius;
	_box.corners[5].x = post_radius;
	_box.corners[6].x = post_radius;

	// create geometry and VAO of the inside of light bulb
	amesh = createSphere(0.2f, 20);
	memcpy(amesh.mat.ambient, amb2, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslations.push_back({ 0, height, 0 });
	_modelRotations.push_back({ 0.0f, 0.0f, 1.0f, 0.0f });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	// create geometry and VAO of the light bulb
	amesh = createSphere(bulb_radius, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff2, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslations.push_back({ 0, height, 0 });
	_modelRotations.push_back({ 0.0f, 0.0f, 1.0f, 0.0f });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	// create geometry and VAO of the solar panels
	float sides = 1.0f, thickness = 0.05f;
	amesh = createCube();
	memcpy(amesh.mat.ambient, amb1, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff1, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslations.push_back({ - sides/2, height + bulb_radius, - sides / 2 });
	_modelRotations.push_back({ 0.0f, 0.0f, 1.0f, 0.0f });
	_modelScales.push_back({ sides, thickness, sides });

	for (int i = 4; i < 8; i++) {
		_box.corners[i].y = height + bulb_radius + thickness;
	}

	_box.corners[0].z = -post_radius;
	_box.corners[1].z = -post_radius;
	_box.corners[4].z = -post_radius;
	_box.corners[5].z = -post_radius;

	_box.corners[2].z = post_radius;
	_box.corners[3].z = post_radius;
	_box.corners[6].z = post_radius;
	_box.corners[7].z = post_radius;

}

void Beacon::createBeacon(float posX, float posZ) {
	_posX = posX;
	_posZ = posZ;

	updateMinMax();
}

void Beacon::render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId) {
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

		// Preform mesh-specific transformations
		translate(MODEL, _modelTranslations[objId].x, _modelTranslations[objId].y, _modelTranslations[objId].z);
		rotate(MODEL, _modelRotations[objId].angle, _modelRotations[objId].x, _modelRotations[objId].y, _modelRotations[objId].z);
		scale(MODEL, _modelScales[objId].x, _modelScales[objId].y, _modelScales[objId].z);

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

void Beacon::updateMinMax() {
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

MinMaxBox Beacon::getMinMax() {
	return _minMax;
}