#version 450 core
layout( local_size_x = 1024, local_size_y = 1, local_size_z = 1) in;

struct Agent {
    vec2 positon;
	vec4 mask;
    float angle;
	int speciesIndex;
};

float moveSpeed = 30.0;
float turnSpeed = 2.0;
float sensorAngleDegrees = 30.0;
float sensorOffsetDst = 35.0;
int sensorSize = 1;
const int numAgents = 250;

layout( std430, binding = 3 ) buffer Agents
{
    Agent Ag[numAgents];
};

layout (rgba32f)  uniform image2D boardImage;
//
uniform float deltaTime;
uniform float currentFrame;

//uniform float trailWeight;
uniform float width;
uniform float height;


float sense(Agent agent, float sensorAngleOffset) {
	float sensorAngle = agent.angle + sensorAngleOffset;
	vec2 sensorDir = vec2(cos(sensorAngle), sin(sensorAngle));

	vec2 sensorPos = agent.positon + sensorDir * sensorOffsetDst;
	int sensorCentreX = int(sensorPos.x);
	int sensorCentreY = int(sensorPos.y);

	float sum = 0;

	vec4 senseWeight = agent.mask * 2 - 1;

	for (int offsetX = -sensorSize; offsetX <= sensorSize; offsetX ++) {
		for (int offsetY = -sensorSize; offsetY <= sensorSize; offsetY ++) {
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
    for(int i = 0; i < numAgents; i++)
    {
        ivec2 pos = ivec2(Ag[i].positon);
        //ivec2 pos = ivec2(0, 0);
    

		ivec2 npos = ivec2(pos.x, pos.y);
		imageStore(boardImage, npos ,vec4(1.0, 1.0, 1.0, 1.0));
    }

	//
    if (gl_GlobalInvocationID.x >= numAgents) 
	{
		return;
	}
	//
	//
	Agent agent = Ag[gl_GlobalInvocationID.x];
	
	vec2 pos = agent.positon;
	
	uint random = hash(uint(pos.y * width + pos.x + hash(uint(gl_GlobalInvocationID.x + currentFrame * 100000))));
	
	float sensorAngleRad = sensorAngleDegrees * (3.1415 / 180);
	float weightForward = sense(agent,0);
	float weightLeft = sense(agent, sensorAngleRad);
	float weightRight = sense(agent, -sensorAngleRad);


	float randomSteerStrength = scaleToRange01(random);
	float turnSpeed = turnSpeed * 2 * 3.1415;

	// Continue in same direction
	if (weightForward > weightLeft && weightForward > weightRight) {
		Ag[gl_GlobalInvocationID.x].angle += 0;
	}
	else if (weightForward < weightLeft && weightForward < weightRight) {
		Ag[gl_GlobalInvocationID.x].angle += (randomSteerStrength - 0.5) * 2 * turnSpeed * deltaTime;
	}
	// Turn right
	else if (weightRight > weightLeft) {
		Ag[gl_GlobalInvocationID.x].angle -= randomSteerStrength * turnSpeed * deltaTime;
	}
	// Turn left
	else if (weightLeft > weightRight) {
		Ag[gl_GlobalInvocationID.x].angle += randomSteerStrength * turnSpeed * deltaTime;
	}
	vec2 direction = vec2(cos(agent.angle), sin(agent.angle));
	vec2 newPos = agent.positon + direction * deltaTime * moveSpeed;

	
	// Clamp position to map boundaries, and pick new random move dir if hit boundary
	if (newPos.x < 0 || newPos.x >= width || newPos.y < 0 || newPos.y >= height) {
		random = hash(random);
		float randomAngle = scaleToRange01(random) * 2 * 3.1415;

		newPos.x = min(width-1,max(0, newPos.x));
		newPos.y = min(height-1,max(0, newPos.y));
		Ag[gl_GlobalInvocationID.x].angle = randomAngle;
	}
	else {
		//vec4 oldTrail = TrailMap[int2(newPos)];
		//TrailMap[ivec2(newPos)] = min(1, oldTrail + agent.speciesMask * trailWeight * deltaTime);
	}
	Ag[gl_GlobalInvocationID.x].positon = newPos;
}