#version 330 core

layout ( location = 0 ) out vec4 fragmentNormal;
layout ( location = 1 ) out vec4 fragmentWorldCoordinate;
layout ( location = 2 ) out vec4 fragmentIntensity;

uniform float lightIntensity;

in vec3 worldSpaceNormal;
in vec4 worldSpacePos;

void main()
{	
	fragmentIntensity = vec4(lightIntensity, lightIntensity, lightIntensity, lightIntensity);
	fragmentNormal = vec4(normalize(worldSpaceNormal),1.0);
	fragmentWorldCoordinate = worldSpacePos;
}