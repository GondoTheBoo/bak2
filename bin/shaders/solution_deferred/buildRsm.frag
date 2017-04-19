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
	fragmentIntensity = vec4(kD * lightIntensity / 5000, 1.0);	
	
	fragmentNormal = vec4( 0.5*N + vec3( 0.5 ), 0.0 ); 
	fragmentWorldCoordinate = worldSpacePos;
}