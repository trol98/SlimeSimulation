#version 450 core
layout( local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Agent {
    vec2 positon;
	vec4 mask;
    float angle;
	int speciesIndex;
};

float moveSpeed = 3.0f ;
float turnSpeed = 5.0f;
float sensorAngleDegrees = 45.0f;
float sensorOffsetDst = 0.5;
int sensorSize = 1;
float numAgents = 50;

layout( std430, binding = 3 ) buffer Agents
{
    Agent Ag[50];
};

layout (rgba32f)  uniform image2D boardImage;

uniform float deltaTime;
uniform float currentFrame;

uniform float trailWeight;
uniform float decayRate;
uniform float diffuseRate;
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
    // i guess the output of this shader should be an red square in the left bottom corner
    //for(int i = 0; i < 50; i++)
    //{
    //    ivec2 pos = ivec2(Ag[i].positon);
    //    //ivec2 pos = ivec2(0, 0);
    //
    //    for(int j = -3; j <= 3; j++)
    //    {
    //        for(int k = -3; k <= 3; k++)
    //        {
    //            ivec2 npos = ivec2(pos.x + j, pos.y + k);
    //            imageStore(boardImage, npos ,vec4(1.0, 0.0, 0.0, 0.2));
    //            
    //        }
    //    }
    //}
    //Positions[0].xyzw=imageLoad(HeightMap, pos).rgba;
	//
    //	if (gl_LocalInvocationID.x >= numAgents) {
	//	return;
	//}
	//
	//
	if(gl_LocalInvocationID.x <= 50)
	{
		Agent agent = Agents[gl_LocalInvocationID.x];
	}
	
	//vec2 pos = agent.position;
	vec2 pos = vec2(0, 0);
	
	//uint random = hash(pos.y * width + pos.x + hash(gl_LocalInvocationID.x + currentFrame * 100000));
	
	// Steer based on sensory data
	float sensorAngleRad = sensorAngleDegrees * (3.1415 / 180);
	//float weightForward = sense(agent,0);
	//float weightLeft = sense(agent, sensorAngleRad);
	//float weightRight = sense(agent, -sensorAngleRad);
	//
	//

}