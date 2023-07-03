#include "Rover.h"

#define M_PI       3.14159265358979323846f

/// The storage for matrices
extern float mMatrix[COUNT_MATRICES][16];
extern float mCompMatrix[COUNT_COMPUTED_MATRICES][16];

/// The normal matrix
extern float mNormal3x3[9];

Rover::Rover() : _posX(2.0f), _posZ(2.0f), _angle(45.0f), _wheelRotAngle(0.f), _wheelSpdAngle(0.f), _speedVec({ 0.0f, (float)sin((_angle + 90) * 3.14f / 180.0f),
	(float)cos((_angle + 90) * 3.14f / 180.0f) }), _moveState(0), _rotateState(0) {
}

float Rover::getSpeed() {
	return _speedVec.speed;
}

void Rover::init() {
	MyMesh amesh;
	float sides, height, radius;
	float y_distance = 0.8f;
	for (int i = 0; i < 4; i++) {
		_box.corners[i].y = 0;
	}

	float amb[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float diff[] = { 0.8f, 0.1f, 0.1f, 1.0f };
	float spec[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float emissive[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float shininess = 500.0f;
	int texcount = 0;

	// create geometry and VAO of the rover body
	//objId = 0
	sides = 1.5f; 
	float body_height = 0.6f;
	amesh = createCube();
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslations.push_back({ -sides / 2, y_distance, -sides / 2 });
	_modelRotations.push_back({ 0.0f, 0.0f, 1.0f, 0.0f });
	_modelScales.push_back({ sides, body_height, sides });

	_box.corners[1].x = sides / 2;
	_box.corners[2].x = sides / 2;
	_box.corners[5].x = sides / 2;
	_box.corners[6].x = sides / 2;

	_box.corners[0].x = -sides / 2;
	_box.corners[3].x = -sides / 2;
	_box.corners[4].x = -sides / 2;
	_box.corners[7].x = -sides / 2;

	// create geometry and VAO of the rover neck
	//objId = 1
	radius = 0.1f; 
	float neck_height = 0.9f;
	amesh = createCylinder(0.9f, 0.1f, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslations.push_back({ 0.5f, y_distance + body_height + neck_height / 2, 0 });
	_modelRotations.push_back({ 0.0f, 0.0f, 1.0f, 0.0f });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	// create geometry and VAO of the rover head
	//objId = 2
	sides = 0.5f; height = 0.2f;
	amesh = createCube();
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	_modelMeshes.push_back(amesh);
	_modelTranslations.push_back({ 0.4f, y_distance + body_height + neck_height, -sides / 2 });
	_modelRotations.push_back({ 0.0f, 0.0f, 1.0f, 0.0f });
	_modelScales.push_back({ sides / 2, height, sides });

	for (int i = 4; i < 8; i++) {
		_box.corners[i].y = y_distance + body_height + neck_height + height;
	}

	// create geometry and VAO of the rover legs
	//objId = 3, 4, 5, 6
	radius = 0.1f; height = 0.96f;
	amesh = createCylinder(height, radius, 20);
	memcpy(amesh.mat.ambient, amb, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	for (int i = 0; i < 4; i++) {
		_modelMeshes.push_back(amesh);
	}
	_modelTranslations.push_back({ 0.5f, (y_distance / 3) * 2, 0.75 });
	_modelRotations.push_back({ -38.66, 1, 0, 0 });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	_modelTranslations.push_back({ 0.5f, (y_distance / 3) * 2, -0.75 });
	_modelRotations.push_back({ 38.66, 1, 0, 0 });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	_modelTranslations.push_back({ -0.5f, (y_distance / 3) * 2, 0.75 });
	_modelRotations.push_back({ -38.66, 1, 0, 0 });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	_modelTranslations.push_back({ -0.5f, (y_distance / 3) * 2, -0.75 });
	_modelRotations.push_back({ 38.66, 1, 0, 0 });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	// create geometry and VAO of the rover wheels
	//objId = 7, 8, 9, 10
	float amb1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	height = 0.2f; radius = 0.2f;
	amesh = createCylinder(height, radius, 20);
	memcpy(amesh.mat.ambient, amb1, 4 * sizeof(float));
	memcpy(amesh.mat.diffuse, diff, 4 * sizeof(float));
	memcpy(amesh.mat.specular, spec, 4 * sizeof(float));
	memcpy(amesh.mat.emissive, emissive, 4 * sizeof(float));
	amesh.mat.shininess = shininess;
	amesh.mat.texCount = texcount;
	for (int i = 0; i < 4; i++) {
		_modelMeshes.push_back(amesh);
	}
	_modelTranslations.push_back({ 0.5f, radius + 0.1f, 1.05f });
	_modelRotations.push_back({ 90, 1, 0, 0 });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	_modelTranslations.push_back({ 0.5f, radius + 0.1f, -1.05f });
	_modelRotations.push_back({ 90, 1, 0, 0 });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	_modelTranslations.push_back({ -0.5f, radius + 0.1f, 1.05f });
	_modelRotations.push_back({ 90, 1, 0, 0 });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	_modelTranslations.push_back({ -0.5f, radius + 0.1f, -1.05f });
	_modelRotations.push_back({ 90, 1, 0, 0 });
	_modelScales.push_back({ 1.0f, 1.0f, 1.0f });

	_box.corners[0].z = -1.05 - height / 2;
	_box.corners[1].z = -1.05 - height / 2;
	_box.corners[4].z = -1.05 - height / 2;
	_box.corners[5].z = -1.05 - height / 2;

	_box.corners[2].z = 1.05 - height / 2;
	_box.corners[3].z = 1.05 - height / 2;
	_box.corners[6].z = 1.05 - height / 2;
	_box.corners[7].z = 1.05 - height / 2;
}

void Rover::reset() {
	_posX = 2.0f;
	_posZ = 2.0f;
	_angle = 45.f;
	_speedVec = { 0.0f, 
		(float)sin((_angle + 90) * 3.14f / 180.0f),
		(float)cos((_angle + 90) * 3.14f / 180.0f) };
	_moveState = 0; 
	_rotateState = 0;
	for (int objId = 7; objId < _modelMeshes.size(); objId++) {
		_modelRotations[objId].angle = 90;
		_modelRotations[objId].x = 1;
		_modelRotations[objId].y = 0;
		_modelRotations[objId].z = 0;
	}
}

Collision Rover::update(float deltaTime, std::vector<MinMaxBox> boxes) {
	float speedLimit = 10, slowDown = 10, accelaration = 30;
	float wheelSpeedFactor = 10.f;
	Collision collision = { -1, 0.0f };
	// Accelerate
	if (_moveState > 0) {
		_speedVec.speed += accelaration * deltaTime;
	} else if (_moveState < 0) {
		_speedVec.speed -= accelaration * deltaTime;
	}

	// Slow down
	if (_speedVec.speed > 0) {
		_speedVec.speed -= slowDown * deltaTime;
		if (_speedVec.speed < 0) _speedVec.speed = 0;
		if (_speedVec.speed > speedLimit) _speedVec.speed = speedLimit;
	}
	else if (_speedVec.speed < 0) {
		_speedVec.speed += slowDown * deltaTime;
		if (_speedVec.speed > 0) _speedVec.speed = 0;
		if (_speedVec.speed < -speedLimit) _speedVec.speed = -speedLimit;
	}
	_wheelSpdAngle += (wheelSpeedFactor / 3) * _speedVec.speed;

	// Rotate
	float oldAngle = _angle;
	float wheelRotateSpeed = 300.f;
	if (_rotateState > 0) {
		_angle += 30.f * deltaTime;
		_speedVec.x = (float)sin((_angle + 90) * 3.14f / 180.0f);
		_speedVec.z = (float)cos((_angle + 90) * 3.14f / 180.0f);
		if (_wheelRotAngle < 45.f)
			_wheelRotAngle += wheelRotateSpeed * deltaTime;
		else
			_wheelRotAngle = 45.f;
		_wheelSpdAngle += wheelSpeedFactor;
	} else if (_rotateState < 0) {
		_angle -= 30.f * deltaTime;
		float weels = _angle + 90;
		_speedVec.x = (float)sin((_angle + 90) * 3.14f / 180.0f);
		_speedVec.z = (float)cos((_angle + 90) * 3.14f / 180.0f);
		if (_wheelRotAngle > -45.f)
			_wheelRotAngle -= wheelRotateSpeed * deltaTime;
		else
			_wheelRotAngle = -45.f;
		_wheelSpdAngle += wheelSpeedFactor;
	}
	else {
		if (_wheelRotAngle < 0.f) {
			_wheelRotAngle += wheelRotateSpeed * deltaTime;
			if (_wheelRotAngle > 0.f) _wheelRotAngle = 0.f;
		}
		else if (_wheelRotAngle > 0.f) {
			_wheelRotAngle -= wheelRotateSpeed * deltaTime;
			if (_wheelRotAngle < 0.f) _wheelRotAngle = 0.f;
		}
	}

	if (_wheelSpdAngle > 360.f) _wheelSpdAngle -= 360.f;
	if (_wheelSpdAngle < -360.f) _wheelSpdAngle += 360.f;

	float newPosX = _posX + (_speedVec.speed * _speedVec.x) * deltaTime;
	float newPosZ = _posZ + (_speedVec.speed * _speedVec.z) * deltaTime;

	updateMinMax(newPosX, newPosZ, _angle);

	for (int i = 0; i < boxes.size(); i++) {
		if (_minMax.min.x <= boxes[i].max.x &&
			_minMax.max.x >= boxes[i].min.x &&
			_minMax.min.z <= boxes[i].max.z &&
			_minMax.max.z >= boxes[i].min.z) {
			collision.id = i;
			collision.speed = abs(_speedVec.speed);
			_speedVec.speed = 0;
			_angle = oldAngle;
			_speedVec.x = (float)sin((_angle + 90) * 3.14f / 180.0f);
			_speedVec.z = (float)cos((_angle + 90) * 3.14f / 180.0f);
			break;
		}
	}

	_posX += (_speedVec.speed * _speedVec.x) * deltaTime;
	_posZ += (_speedVec.speed * _speedVec.z) * deltaTime;

	return collision;
}

void Rover::render(int shader, GLint vm_uniformId, GLint pvm_uniformId, GLint normal_uniformId) {
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
		translate(MODEL, _modelTranslations[objId].x, _modelTranslations[objId].y, _modelTranslations[objId].z);
		if (objId >= 7) {
			// Wheel rotation transformations
			if (objId < 9)
				rotate(MODEL, _wheelRotAngle, 0.f, 1.f, 0.f);
			else
				rotate(MODEL, -_wheelRotAngle, 0.f, 1.f, 0.f);
			// Wheel speed transformations
			rotate(MODEL, _wheelSpdAngle, 0.f, 0.f, -1.f);
		}
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

void Rover::move(int dir) {

	switch (dir) {
	case 0:
		_rotateState = 1;
		break;
	case 1:
		_rotateState = -1;
		break;
	case 2:	//move forwards
		_moveState = 1;
		break;
	case 3: //move backwards
		_moveState = -1;
		break;
	case 4:
		_rotateState = 0;
		break;
	case 5:
		_moveState = 0;
		break;
	}
}

std::array<float, 3> Rover::getFollowTarget() {
	return { _posX, 1.0f, _posZ };
}

std::array<float, 3> Rover::getFollowPos(float alpha, float beta, float r) {
	float camX, camZ, camY;

	camX = r * sin((alpha + _angle) * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camZ = r * cos((alpha + _angle) * 3.14f / 180.0f) * cos(beta * 3.14f / 180.0f);
	camY = r * sin(beta * 3.14f / 180.0f);

	camX += _posX;
	camZ += _posZ;
	camY += 1.0f;

	return { camX, camY, camZ };
}

std::array<float, 4> Rover::getPos() {
	return { _posX, 1.1f, _posZ, 1.0f };
}

RoverLights Rover::getRoverLights() {
	RoverLights lights;

	float offsetX1 = 0.4 * sin(((_angle + 90) + 40) * 3.14f / 180.0f);
	float offsetZ1 = 0.4 * cos(((_angle + 90) + 40) * 3.14f / 180.0f);

	float offsetX2 = 0.4 * sin(((_angle + 90) - 40) * 3.14f / 180.0f);
	float offsetZ2 = 0.4 * cos(((_angle + 90) - 40) * 3.14f / 180.0f);

	lights.pos1 = { _posX + offsetX1, 1.1f, _posZ + offsetZ1, 1.0 };
	lights.pos2 = { _posX + offsetX2, 1.1f, _posZ + offsetZ2, 1.0 };

	lights.dir = { _speedVec.x, 0.0f, _speedVec.z, 0.0f };

	return lights;
}

void Rover::updateMinMax(float posX, float posZ, float angle) {
	float minX, minY, minZ, maxX, maxY, maxZ;

	pushMatrix(MODEL);
	loadIdentity(MODEL);
	translate(MODEL, posX, 0.0f, posZ);
	rotate(MODEL, angle, 0.0f, 1.0f, 0.0f);
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
		translate(MODEL, posX, 0.0f, posZ);
		rotate(MODEL, angle, 0.0f, 1.0f, 0.0f);
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