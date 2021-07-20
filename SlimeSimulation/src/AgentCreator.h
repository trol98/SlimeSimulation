#pragma once

#include "vendor/glm/glm/glm.hpp"
#include "vendor/glm/glm/gtc/matrix_transform.hpp"


#include <cstdlib>
#include <ctime>

#include "Agent.h"

class AgentCreator
{
private:
	static glm::vec2 getPointUnitCircle()
	{
		// https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
		float t = 2 * glm::pi<float>() * ((float)rand() / RAND_MAX);
		float u = ((float)rand() / RAND_MAX) + ((float)rand() / RAND_MAX);
		float r = u > 1 ? 2 - u : u;
		return glm::vec2(r * cos(t), r * sin(t));
	}
public:
	static Agent* createRandom(int numberAgent, int screenWidth, int screenHeight)
	{
		Agent* agentArray = new Agent[numberAgent];
		for (int i = 0; i < numberAgent; i++)
		{
			int randomX = rand() % screenWidth;
			int randomY = rand() % screenHeight;

			agentArray[i] = Agent(glm::vec2(randomX, randomY), glm::vec4(), 0.0f, 0);
		}
		return agentArray;
	}
	static Agent* createPoint(int numberAgent, int screenWidth, int screenHeight)
	{
		Agent* agentArray = new Agent[numberAgent];
		glm::vec2 centre(screenWidth / 2.0f, screenHeight / 2.0f);

		for (int i = 0; i < numberAgent; i++)
		{
			float randomAngle = ((float)rand() / RAND_MAX) * glm::pi<float>() * 2;

			agentArray[i] = Agent(centre, glm::vec4(), randomAngle, 0);
		}
		return agentArray;
	}
	static Agent* createCircle(int numberAgent, int screenWidth, int screenHeight)
	{
		Agent* agentArray = new Agent[numberAgent];

		float randomAngle = ((float)rand() / RAND_MAX) * glm::pi<float>() * 2;
		glm::vec2 centre(screenWidth / 2.0, screenHeight / 2.0);

		for (int i = 0; i < numberAgent; i++)
		{
			glm::vec2 centerPos = centre + getPointUnitCircle() * (float)(screenHeight / 4.0) * 0.15f;

			agentArray[i] = Agent(centerPos, glm::vec4(), randomAngle, 0);
		}
		return agentArray;
	}

};