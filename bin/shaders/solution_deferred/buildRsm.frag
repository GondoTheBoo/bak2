#version 330 core

layout ( location = 0 ) out vec4 fragmentNormal;
layout ( location = 1 ) out vec4 fragmentWorldCoordinate;
layout ( location = 2 ) out vec4 fragmentIntensity;

uniform float lightIntensity;
uniform vec3 lightPos;

const float EPSILON = 0.01;

in vec3 worldSpaceNormal;
in vec4 worldSpacePos;

void main()
{	
	vec3 toLight = lightPos - worldSpacePos.xyz;
	float lightFalloff = 1.0 / length( toLight ); lightFalloff *= lightFalloff; lightFalloff = min( 1, lightFalloff );
	if (lightFalloff * lightIntensity < EPSILON)
		lightFalloff = 0.0;
	
	fragmentIntensity = vec4(lightFalloff, lightFalloff, lightFalloff, 1.0);
	fragmentNormal = vec4(normalize(worldSpaceNormal),1.0);
	fragmentWorldCoordinate = worldSpacePos;
}