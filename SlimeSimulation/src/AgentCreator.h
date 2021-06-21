#pragma once
#include <vector>
#include <memory>

#include <cstdlib>
#include <ctime>

#include "Agent.h"

class AgentCreator
{
public:
	static std::vector<std::unique_ptr<Agent>>* createRandom(int numberAgent, int width, int height)
	{
		srand(time(nullptr));

		std::vector<std::unique_ptr<Agent>>* agents = new std::vector<std::unique_ptr<Agent>>(numberAgent);
		for (int i = 0; i < numberAgent; i++)
		{
			int randomX = rand() % width;
			int randomY = rand() % width;

			agents->at(i) = std::make_unique<Agent>(new Agent(glm::vec2(randomX, randomY), glm::vec4(), 0.0f, 0));
		}
	}

};