#version 330 core

layout ( location = 0 ) in vec3 vertexPosition;
layout ( location = 1 ) in vec3 vertexNormal;

uniform mat4 projectionTf;
uniform mat4 modelviewTf;
uniform mat3 normals2EyeTf;

out vec3 normal;
out vec4 position;

void main()
{
	normal = normalize( normals2EyeTf * vertexNormal );
	position = modelviewTf * vec4( vertexPosition, 1.0 );

	gl_Position = projectionTf * position;
}