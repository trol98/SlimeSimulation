#version 450 core

//layout( std430, binding=1 ) buffer VertBuffer
//    {
//    vec4 Positions[ ]; 
//    };

layout( local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout (rgba32f)  uniform image2D boardImage;

void main (void)
{
    // i guess the output of this shader should be an red square in the left bottom corner
    for(int i = 0; i < 100; i++)
    {
        for(int j = 0; j < 100; j++)
        {
            ivec2 pos=ivec2(i, j);
            imageStore(boardImage, pos,vec4(1.0, 0.0, 0.0, 1.0));
        } 
    }
    //Positions[0].xyzw=imageLoad(HeightMap, pos).rgba;
}