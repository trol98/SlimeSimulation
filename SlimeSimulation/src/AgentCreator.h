#pragma once
#include <vector>
#include <memory>

#include <cstdlib>
#include <ctime>

#include "Agent.h"

class AgentCreator
{
public:
	static Agent* createRandom(int numberAgent, int width, int height)
	{
		srand(time(nullptr));
		Agent* agentArray = new Agent[numberAgent];
		for (int i = 0; i < numberAgent; i++)
		{
			int randomX = rand() % height;
			int randomY = rand() % width;

			agentArray[i] = Agent(glm::vec2(randomX, randomY), glm::vec4(), 0.0f, 0);
		}
		return agentArray;
	}

};