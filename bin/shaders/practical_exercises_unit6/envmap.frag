#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

const int SOLID_COLOR = 0;
const int RENDER_ENVIRONMENT = 1;
const int REFLECTIVE = 2;

struct RasterizerData
{
	vec4 position;
	vec3 normal;
};

in RasterizerData rasterizerData;

uniform sampler2D normalTexture;
uniform bool hasNormalTexture = false;
// if false -> use geometric normal

uniform sampler2D aoTexture;
uniform bool hasAoTexture = false;
// if false -> use ao value of 1

uniform vec4 cameraPosition;
uniform int shadingType =  SOLID_COLOR;

void main()
{	
	if (shadingType == SOLID_COLOR)
	{
		fragmentColor = vec4(1);
	}
	else if (shadingType == RENDER_ENVIRONMENT)
	{
		fragmentColor = vec4(0);
	}
	else
	{
		vec3 N = normalize( rasterizerData.normal );
		vec3 V = normalize( cameraPosition.xyz-rasterizerData.position.xyz );
		fragmentColor = vec4( vec3( max( dot( N, V ), 0 ) ), 1 );
	}
}