#pragma once

#include "vendor/glm/glm/glm.hpp"
#include "vendor/glm/glm/gtc/matrix_transform.hpp"

#include <cstdlib>
#include <ctime>

#include "Agent.h"

class AgentCreator
{
private:

	static float random01()
	{
		return ((float)rand() / RAND_MAX);
	}

	static glm::vec2 getPointUnitCircle()
	{
		// https://stackoverflow.com/questions/5837572/generate-a-random-point-within-a-circle-uniformly
		float t = random01() * glm::pi<float>() * 2.0f;
		return glm::vec2(cos(t), sin(t));
	}
public:

	static Agent* createAgents(unsigned numberAgents, unsigned numberSpecies, AgentMode mode,
		unsigned width, unsigned height)
	{
		Agent* agentArray = new Agent[numberAgents];

		glm::vec2 center(width / 2.0f, height / 2.0f);
		float randomAngle = random01() * glm::pi<float>() * 2.0f;

		glm::vec2 agentPos = center;
		float angle = randomAngle;

		float radius = 0.0f;


		for (unsigned i = 0; i < numberAgents; i++)
		{
			switch (mode)
			{
			case AgentMode::Random:
				// nothing todo
				break;
			case AgentMode::Center:
				agentPos = glm::vec2(rand() % width, rand() % height);
				break;
			case AgentMode::Circle:
				radius = height * 0.15f;
				agentPos = center + getPointUnitCircle() * radius;
				break;
			case AgentMode::InwardCircle:
				radius = height * 0.5f;
				agentPos = center + getPointUnitCircle() * radius;
				glm::vec2 centerDirection = glm::normalize(center - agentPos);
				angle = glm::atan(centerDirection.y, centerDirection.x);
				break;
			}

			glm::vec4 speciesMask;
			int speciesIndex = 0;

			if (numberSpecies == 1)
			{
				speciesMask = glm::vec4(1, 1, 1, 1);
			}
			else
			{
				int species = int(random01() * numberSpecies) + 1;
				speciesIndex = species - 1;
				speciesMask = glm::vec4((species == 1) ? 1 : 0, (species == 2) ? 1 : 0, (species == 3) ? 1 : 0, 1);
			}
			agentArray[i] = { agentPos, speciesMask, angle, speciesIndex };
		}
		return agentArray;
	}


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
			float randomAngle = random01() * glm::pi<float>() * 2;

			agentArray[i] = Agent(centre, glm::vec4(), randomAngle, 0);
		}
		return agentArray;
	}
	static Agent* createCircle(int numberAgent, int screenWidth, int screenHeight)
	{
		Agent* agentArray = new Agent[numberAgent];

		float randomAngle = random01() * glm::pi<float>() * 2.0f;
		glm::vec2 centre(screenWidth / 2.0, screenHeight / 2.0);

		for (int i = 0; i < numberAgent; i++)
		{
			glm::vec2 centerPos = centre + getPointUnitCircle() * (float)(screenHeight / 4.0) * 0.15f;

			agentArray[i] = Agent(centerPos, glm::vec4(), randomAngle, 0);
		}
		return agentArray;
	}

};