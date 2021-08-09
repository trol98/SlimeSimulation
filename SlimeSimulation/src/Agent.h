#pragma once
#include "vendor/glm/glm/glm.hpp"

struct Agent
{
	glm::vec2 positon;
	glm::ivec3 mask;
	float angle;
	int speciesIndex;

	Agent() = default;

	Agent(glm::vec2 p, glm::ivec3 m, float a, int s)
		:positon(p), mask(m), angle(a), speciesIndex(s)
	{
	}
};

struct SpeciesSettings
{
	float moveSpeed;
	float turnSpeed;
	float sensorAngle;
	float sensorOffset;
	int sensorSize;
	float r;
	float g;
	float b;

	SpeciesSettings() = default;

	SpeciesSettings(float moveSpeed, float turnSpeed, float sensorAngle, float sensorOffset, int sensorSize, float r, float g, float b)
		:moveSpeed(moveSpeed), turnSpeed(turnSpeed), sensorAngle(sensorAngle),
		 sensorOffset(sensorOffset), sensorSize(sensorSize), r(r), g(g), b(b)
	{
	}
};

enum class AgentMode
{
	Random,
	Center,
	Circle,
	InwardCircle
};





