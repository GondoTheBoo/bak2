#version 330 core

layout ( location = 0 ) in vec3 vertexPosition;
layout ( location = 8 ) in vec4 vertexColor;

uniform mat4 projectionTf;
uniform mat4 viewTf;
uniform mat4 modelTf;

out vec4 color;

void main()
{
	color = vertexColor;
	gl_Position = projectionTf * viewTf * modelTf * vec4( vertexPosition, 1.0 );
}