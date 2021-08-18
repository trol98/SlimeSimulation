#version 450 core

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D textureBoard;
void main()
{
	FragColor = texture(textureBoard, TexCoords);
}