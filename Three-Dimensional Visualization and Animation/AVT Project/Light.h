#pragma once

#define _USE_MATH_DEFINES 1

#include <math.h>
#include <array>
#include <string>
#include <iostream>
#include <GL/glew.h>

#include "AVTmathLib.h"

class Light {
public:	
	Light() : _programID(0), _uniformPos(""), _enabled(true) {}
	virtual ~Light() {}
	virtual void updateView() = 0;
	virtual void reflect() = 0;
	void lightSwitch();
	bool getEnabled();
protected:
	int _programID;
	std::string _uniformPos;
	bool _enabled;
	void initShader(int programID, std::string uniformPos);
};

class PointLight : public Light {
public:
	PointLight() : Light(), _pos(), _range(0.0f) {}
	void init(int programID, std::string uniformPos, std::array<float, 4> pos, float range);
	virtual void updateView();
	virtual void reflect();
private:
	std::array<float, 4> _pos;
	float _range;
};

class SpotLight : public Light {
public:
	SpotLight() : Light(), _pos(), _dir(), _range(0.0f), _angle(0.0f) {}
	void init(int programID, std::string uniformPos, std::array<float, 4> pos, std::array<float, 4> dir, float range, float angle);
	virtual void updateView();
	virtual void reflect();
	void moveLight(std::array<float, 4> pos, std::array<float, 4> dir);
private:
	std::array<float, 4> _pos;
	std::array<float, 4> _dir;
	float _range;
	float _angle;
};

class DirLight : public Light {
public:
	DirLight() : Light(), _dir() {}
	void init(int programID, std::string uniformPos, std::array<float, 4> dir);
	virtual void updateView();
	virtual void reflect();
	std::array<float, 4> calcSrcPos(std::array<float, 4> pos);
private:
	std::array<float, 4> _dir;
};

