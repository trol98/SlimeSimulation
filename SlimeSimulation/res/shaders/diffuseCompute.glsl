#version 450 core
layout( local_size_x = 256, local_size_y = 1, local_size_z = 1) in;

uniform float decayRate;
uniform float diffuseRate;
uniform float width;
uniform float height;
uniform float deltaTime;

layout (rgba32f)  uniform image2D boardImage;

float evaporateSpeed = 0.3;

void main()
{
	ivec3 globalID = ivec3(gl_GlobalInvocationID);

	if(globalID.x < 0 || globalID.x >= uint(width) ||
	   globalID.y < 0 || globalID.y >= uint(height))
		return;
	
	vec4 sum = vec4(0.0);
	vec4 originalCol = imageLoad(boardImage, ivec2(globalID.x, globalID.y));

	// 3x3 blur
	for (int offsetX = -1; offsetX <= 1; offsetX++)
	{
		for (int offsetY = -1; offsetY <= 1; offsetY++) 
		{
			int sampleX = int(min(width-1, max(0, globalID.x +  offsetX)));
			int sampleY = int(min(height-1, max(0, globalID.y + offsetY)));
			// 
			sum += imageLoad(boardImage, ivec2(sampleX, sampleY));
		}
	}

	vec4 blurredCol = sum / 9;
	float diffuseWeight = clamp(diffuseRate * deltaTime, 0.0, 1.0);
	blurredCol = originalCol * (1 - diffuseWeight) + blurredCol * diffuseWeight;

	//imageStore(boardImage, ivec2(gl_LocalInvocationID.x, gl_LocalInvocationID.y), vec4(max(0, int(blurredCol - decayRate * deltaTime))));
	//imageStore(boardImage, globalID.xy, blurredCol - vec4(decayRate * deltaTime));
	//imageStore(boardImage, ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y), vec4(1.0,0.0,0.0,1.0));
	imageStore(boardImage, globalID.xy, max(vec4(0.0), blurredCol - decayRate * deltaTime));
}