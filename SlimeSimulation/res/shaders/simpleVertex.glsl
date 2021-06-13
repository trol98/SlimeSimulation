#version 450 core
in layout(location = 0) vec3 aPos;
in layout(location = 1) vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 perspectiveMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main()
{
	TexCoords = aTexCoords;
	//gl_Position = perspectiveMatrix * viewMatrix * modelMatrix * vec4(aPos, 1.0);
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}