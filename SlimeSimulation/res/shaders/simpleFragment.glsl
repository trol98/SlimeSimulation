#version 450 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D textureBoard;
void main()
{
	//FragColor = vec4(1.0, 0.5, 0.2, 1.0);
	FragColor = texture(textureBoard, TexCoords);
}