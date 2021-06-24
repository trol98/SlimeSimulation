#version 450 core
layout( local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

struct Agent {
    vec2 Positon;
	vec4 Mask;
    float Angle;
	int SpeciesIndex;
};

layout( std430, binding = 3 ) buffer Agents
{
    Agent Ag[50];
};

layout (rgba32f)  uniform image2D boardImage;

void main (void)
{
    // i guess the output of this shader should be an red square in the left bottom corner
    for(int i = 0; i < 50; i++)
    {
        ivec2 pos = ivec2(Ag[i].Positon);
        //ivec2 pos = ivec2(0, 0);

        for(int j = -3; j <= 3; j++)
        {
            for(int k = -3; k <= 3; k++)
            {
                ivec2 npos = ivec2(pos.x + j, pos.y + k);
                imageStore(boardImage, npos ,vec4(1.0, 0.0, 0.0, 1.0));
                
            }
        }
    }
    //Positions[0].xyzw=imageLoad(HeightMap, pos).rgba;
}