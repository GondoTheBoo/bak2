#version 330 core

layout ( location = 0 ) in vec3 vertexPosition;

uniform float time = 0;

void main()
{
	gl_Position = vec4( vertexPosition, 1.0 );
}