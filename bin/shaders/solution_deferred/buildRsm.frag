#version 330 core

layout ( location = 0 ) out vec4 fragmentNormal;
layout ( location = 1 ) out vec4 fragmentWorldCoordinate;
layout ( location = 2 ) out vec4 fragmentIntensity;

uniform float lightIntensity; //flux
uniform vec3 lightPos;

const float EPSILON = 0.01;

in vec3 worldSpaceNormal;
in vec4 worldSpacePos;
in vec2 uv;

uniform sampler2D diffuseTexture;
uniform vec3 kDiffuse = vec3( 0.5 );
uniform bool hasDiffuseTexture = false;

void main()
{	
	vec3 N = normalize(worldSpaceNormal);
	vec3 kD = kDiffuse;
	if ( hasDiffuseTexture )
	{
		kD = texture( diffuseTexture, uv ).rgb;
	}
	vec3 toLight = lightPos - worldSpacePos.xyz;
	float lightFalloff = 1.0 / length( toLight ); lightFalloff *= lightFalloff; lightFalloff = min( 1, lightFalloff );
	
	fragmentIntensity = vec4(lightFalloff * kD * lightIntensity, 1.0);	
	fragmentIntensity /= 100;
	fragmentNormal = vec4( 0.5*N + vec3( 0.5 ), 0.0 ); 
	fragmentWorldCoordinate = worldSpacePos;
}