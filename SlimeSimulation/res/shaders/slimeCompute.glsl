#version 450 core
layout( local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

uniform float deltaTime;
uniform float currentFrame;
uniform float width;
uniform float height;

//uniform float moveSpeed;
//uniform float turnSpeed;
//uniform float sensorAngleDegrees;
//uniform float sensorOffsetDst;
//uniform int sensorSize;

//uniform float trailWeight;
// max number of agents on my computer is 2^16 - 1
const int numAgents = 65535;
const int numSettings = 3;

struct Agent {
    vec2 positon;
	vec4 mask;
    float angle;
	int speciesIndex;
};

struct SpeciesSettings {
	float moveSpeed;
	float turnSpeed;

	float sensorAngleDegrees;
	float sensorOffsetDst;
	float sensorSize;

	float r;
	float g;
	float b;
};

layout( std140, binding = 0 ) buffer Agents
{
    Agent Ag[numAgents];
};

layout( std140, binding = 1 ) buffer Spec
{
    SpeciesSettings specSettings[numSettings];
};

//uniform SpeciesSettings specSettings[numSettings];

layout (rgba32f)  uniform image2D boardImage;

float sense(Agent agent, SpeciesSettings settings, float sensorAngleOffset) {
	float sensorAngle = agent.angle + settings.sensorAngleDegrees;
	vec2 sensorDir = vec2(cos(sensorAngle), sin(sensorAngle));

	vec2 sensorPos = agent.positon + sensorDir * settings.sensorOffsetDst;
	int sensorCentreX = int(sensorPos.x);
	int sensorCentreY = int(sensorPos.y);

	float sum = 0;
	vec4 senseWeight = agent.mask * 2 - 1;
	int sensorSize = int(settings.sensorSize);

	for (int offsetX = -sensorSize; offsetX <= sensorSize; offsetX++) {
		for (int offsetY = -sensorSize; offsetY <= sensorSize; offsetY++) {
			int sampleX = int(min(width - 1, max(0, sensorCentreX + offsetX)));
			int sampleY = int(min(height - 1, max(0, sensorCentreY + offsetY)));
			sum += dot(senseWeight, imageLoad(boardImage, ivec2(sampleX, sampleY)));
		}
	}
	return sum;
}

// www.cs.ubc.ca/~rbridson/docs/schechter-sca08-turbulence.pdf
uint hash(uint state)
{
    state ^= 2747636419u;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    state ^= state >> 16;
    state *= 2654435769u;
    return state;
}
float scaleToRange01(uint state)
{
    return state / 4294967295.0;
}

void main (void)
{	
	ivec3 globalID = ivec3(gl_GlobalInvocationID);



    if (globalID.x >= numAgents) 
	{
		return;
	}

	// calculating part
	Agent agent = Ag[globalID.x];
	SpeciesSettings settings = specSettings[int(agent.speciesIndex)];
	
	vec2 pos = agent.positon;
	
	uint random = hash(uint(pos.y * width + pos.x + hash(uint(globalID.x + currentFrame * 100000))));
	
	float sensorAngleRad = settings.sensorAngleDegrees * (3.1415 / 180);
	float weightForward = sense(agent,settings, 0);
	float weightLeft = sense(agent,settings, sensorAngleRad);
	float weightRight = sense(agent,settings, -sensorAngleRad);


	float randomSteerStrength = scaleToRange01(random);
	float turnSpeed = settings.turnSpeed * 2 * 3.1415;

	// Continue in same direction
	//if (weightForward > weightLeft && weightForward > weightRight) {
		//Ag[gl_GlobalInvocationID.x].angle += 0;
	//}
	/*else*/if (weightForward < weightLeft && weightForward < weightRight) {
		Ag[globalID.x].angle += (randomSteerStrength - 0.5) * 2 * turnSpeed * deltaTime;
	}
	// Turn right
	else if (weightRight > weightLeft) {
		Ag[globalID.x].angle -= randomSteerStrength * turnSpeed * deltaTime;
	}
	// Turn left
	else if (weightLeft > weightRight) {
		Ag[globalID.x].angle += randomSteerStrength * turnSpeed * deltaTime;
	}




	vec2 direction = vec2(cos(agent.angle), sin(agent.angle));
	vec2 newPos = agent.positon + direction * deltaTime * settings.moveSpeed;
	
	// Clamp position to map boundaries, and pick new random move dir if hit boundary
	if (newPos.x < 0 || newPos.x >= width || newPos.y < 0 || newPos.y >= height) {
		random = hash(random);
		float randomAngle = scaleToRange01(random) * 2 * 3.1415;

		newPos.x = min(width-1,max(0, newPos.x));
		newPos.y = min(height-1,max(0, newPos.y));
		Ag[globalID.x].angle = randomAngle;
	}

	Ag[globalID.x].positon = newPos;

	ivec2 pixelPos = ivec2(Ag[globalID.x].positon);

	vec4 color = vec4(settings.r, settings.g, settings.b, 1.0);

	imageStore(boardImage, pixelPos, color);
}