#version 330 core

layout ( location = 0 ) out vec4 fragmentNormal;
layout ( location = 1 ) out vec4 fragmentDiffuse;
layout ( location = 2 ) out vec4 fragmentSpecular;
layout ( location = 3 ) out vec4 fragmentAmbient;

const float MAX_SHININESS = 1000.0;

struct RasterizerData
{
	vec3 normal;
	vec2 uv;
};

in RasterizerData rasterizerData;

uniform sampler2D diffuseTexture;
uniform vec3 kDiffuse = vec3( 0.5 );
uniform bool hasDiffuseTexture = false;

uniform sampler2D specularTexture;
uniform vec3 kSpecular = vec3( 0.5 );
uniform bool hasSpecularTexture = false;

uniform sampler2D ambientTexture;
uniform vec3 kAmbient = vec3( 0.5 );
uniform bool hasAmbientTexture = false;

uniform float shininess = 4.0;

uniform int solidFlag = 0;

void main()
{	
	vec3 kD = kDiffuse;
	if ( hasDiffuseTexture )
	{
		kD = texture( diffuseTexture, rasterizerData.uv ).rgb;
	}

	vec3 kS = kSpecular;
	if ( hasSpecularTexture )
	{
		kS = texture( specularTexture, rasterizerData.uv ).rgb;
	}

	vec3 kA = kAmbient;
	if ( hasAmbientTexture )
	{
		kA = texture( ambientTexture, rasterizerData.uv ).rgb;
	}
	
	vec3 N = normalize( rasterizerData.normal );

	fragmentNormal = vec4( 0.5*N + vec3( 0.5 ), 0.0 );
	fragmentDiffuse = vec4( kD, solidFlag );
	fragmentSpecular = vec4( kS, min(shininess, MAX_SHININESS) / MAX_SHININESS );
	fragmentAmbient = vec4( kA, 0.0 );
}