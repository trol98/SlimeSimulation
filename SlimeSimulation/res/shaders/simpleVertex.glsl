#version 450 core
in layout(location = 0) vec3 aPos;
in layout(location = 1) vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = vec4(aPos, 1.0);
}