#version 330 core

layout ( location = 0 ) in vec3 vertexPosition;
layout ( location = 1 ) in vec3 vertexNormal;

uniform mat4 modelTf;
uniform mat4 mvpTf;
uniform mat3 normals2worldTf;

out vec4 position;		// world space
out vec3 normal;		// world space

void main()
{
	position = modelTf * vec4(vertexPosition, 1);
	normal = normalize(normals2worldTf * vertexNormal);

	gl_Position = mvpTf * vec4(vertexPosition, 1);
}