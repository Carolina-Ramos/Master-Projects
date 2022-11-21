#include "Light.h"

void Light::initShader(int programID, std::string uniformPos) {
	_programID = programID; _uniformPos = uniformPos;
}

void Light::lightSwitch() {
	if (_enabled)
		_enabled = false;
	else _enabled = true;
}

bool Light::getEnabled() {
	return _enabled;
}

void PointLight::init(int programID, std::string uniformPos, std::array<float, 4> pos, float range) {
	initShader(programID, uniformPos);
	_pos = pos; _range = range;

	std::string uniformStr; int uniformIndex;

	uniformStr = _uniformPos + ".position";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform4fv(uniformIndex, 1, _pos.data());

	uniformStr = _uniformPos + ".range";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1f(uniformIndex, _range);
	
	uniformStr = _uniformPos + ".enabled";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1i(uniformIndex, _enabled);
}

void PointLight::updateView() {
	float res[4];
	multMatrixPoint(VIEW, _pos.data(), res);

	std::string uniformStr; int uniformIndex;

	uniformStr = _uniformPos + ".position";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform4fv(uniformIndex, 1, res);

	uniformStr = _uniformPos + ".range";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1f(uniformIndex, _range);

	uniformStr = _uniformPos + ".enabled";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1i(uniformIndex, _enabled);
}

void PointLight::reflect() {
	_pos[1] *= -1;
}

void SpotLight::init(int programID, std::string uniformPos, std::array<float, 4> pos, std::array<float, 4> dir, float range, float angle) {
	initShader(programID, uniformPos);
	_pos = pos; _dir = dir; _range = range; _angle = angle * (M_PI / 180);

	std::string uniformStr; int uniformIndex;

	uniformStr = _uniformPos + ".position";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform4fv(uniformIndex, 1, _pos.data());

	uniformStr = _uniformPos + ".direction";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform4fv(uniformIndex, 1, _dir.data());

	uniformStr = _uniformPos + ".range";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1f(uniformIndex, _range);

	uniformStr = _uniformPos + ".angle";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1f(uniformIndex, _angle);

	uniformStr = _uniformPos + ".enabled";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1i(uniformIndex, _enabled);
}

void SpotLight::updateView() {
	float resPos[4], resDir[4];
	multMatrixPoint(VIEW, _pos.data(), resPos);
	multMatrixPoint(VIEW, _dir.data(), resDir);

	std::string uniformStr; int uniformIndex;

	uniformStr = _uniformPos + ".position";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform4fv(uniformIndex, 1, resPos);

	uniformStr = _uniformPos + ".direction";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform4fv(uniformIndex, 1, resDir);

	uniformStr = _uniformPos + ".range";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1f(uniformIndex, _range);

	uniformStr = _uniformPos + ".angle";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1f(uniformIndex, _angle);

	uniformStr = _uniformPos + ".enabled";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1i(uniformIndex, _enabled);
}

void SpotLight::reflect() {
	_pos[1] *= -1;
	_dir[1] *= -1;
}

void SpotLight::moveLight(std::array<float, 4> pos, std::array<float, 4> dir) {
	_pos = pos; _dir = dir;
}

void DirLight::init(int programID, std::string uniformPos, std::array<float, 4> dir) {
	initShader(programID, uniformPos);
	float dirData[] = { dir[0], dir[1], dir[2] };
	normalize(dirData);
	_dir = { dirData[0], dirData[1], dirData[2], 0.f };

	std::string uniformStr; int uniformIndex;

	uniformStr = _uniformPos + ".direction";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform4fv(uniformIndex, 1, _dir.data());

	uniformStr = _uniformPos + ".enabled";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1i(uniformIndex, _enabled);
}

void DirLight::updateView() {
	float res[4];
	multMatrixPoint(VIEW, _dir.data(), res);
	
	std::string uniformStr; int uniformIndex;

	uniformStr = _uniformPos + ".direction";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform4fv(uniformIndex, 1, res);

	uniformStr = _uniformPos + ".enabled";
	uniformIndex = glGetUniformLocation(_programID, uniformStr.c_str());
	glUniform1i(uniformIndex, _enabled);
}

void DirLight::reflect() {
	_dir[1] *= -1;
}

std::array<float, 4> DirLight::calcSrcPos(std::array<float, 4> pos) {
	std::array<float, 4> srcPos;
	
	srcPos[0] = pos[0] - _dir[0] * 500;
	srcPos[1] = pos[1] - _dir[1] * 500;
	srcPos[2] = pos[2] - _dir[2] * 500;
	srcPos[3] = pos[3];

	return srcPos;
}
