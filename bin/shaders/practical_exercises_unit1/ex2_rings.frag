#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

const float PI = 3.14159265359;

uniform float time = 0;

void main()
{	
	fragmentColor = vec4( abs(cos(time)), abs(cos(time + PI/3)), abs(cos(time + 2*PI/3)), 1.0 );
}