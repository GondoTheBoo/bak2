#version 330 core

layout ( location = 0 ) out vec4 fragmentNormal;
layout ( location = 1 ) out vec4 fragmentDiffuse;
layout ( location = 2 ) out vec4 fragmentWorldCoordinate;
layout ( location = 3 ) out vec4 fragmentAmbient;

struct RasterizerData
{
	vec3 normal;
	vec2 uv;
};

in RasterizerData rasterizerData;
in vec4 worldSpacePos;

uniform sampler2D diffuseTexture;
uniform vec3 kDiffuse = vec3( 0.5 );
uniform bool hasDiffuseTexture = false;

void main()
{	
	vec3 kD = kDiffuse;
	if ( hasDiffuseTexture )
	{
		kD = texture( diffuseTexture, rasterizerData.uv ).rgb;
	}	
	vec3 N = normalize( rasterizerData.normal );

	fragmentNormal = vec4( 0.5*N + vec3( 0.5 ), 0.0 );
	fragmentDiffuse = vec4( kD, 1 );
	fragmentWorldCoordinate = worldSpacePos;
	
	//can delete once paramter removed
	fragmentAmbient = vec4( 0.0,0.0,0.0, 0.0 );
}