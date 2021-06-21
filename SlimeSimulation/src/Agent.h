#pragma once
#include "vendor/glm/glm/glm.hpp"

struct Agent
{
	glm::vec2 positon;
	glm::vec4 mask;
	float angle;
	int speciesIndex;

	Agent(glm::vec2 p, glm::vec4 m, float a, int s)
		:positon(p), mask(m), angle(a), speciesIndex(s)
	{

	}
};





