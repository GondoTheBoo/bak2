#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

const float PI = 3.14159265359;
const int MAX_LIGHTS = 100;
const float EPSILON = 0.01;

struct RasterizerData
{
	vec4 position;
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

struct LightSource
{
	uint type;
	float intensity;
	vec3 color;
	vec4 position;
	vec3 direction;
	float cosangle;
};

uniform LightSource[MAX_LIGHTS] lightSources;
uniform int lightCount = 0;

uniform float ambientIntensity = 0.1;
uniform vec3 ambientColor = vec3( 1.0 );

uniform int solidFlag = 0;

void main()
{	
	if (solidFlag == 1)
	{
		fragmentColor = vec4(kDiffuse, 1);
		return;
	}

	vec3 N = normalize( rasterizerData.normal );
	vec3 V = normalize( -rasterizerData.position.xyz );

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
	
	vec3 color = ambientIntensity * ambientColor * kA;
	for (int i=0; i<min(lightCount, MAX_LIGHTS); ++i)
	{
		float lightFalloff = 1.0;
		float angularCutoff = 1.0;
		vec3 L = vec3( 0.0 );
		if ( lightSources[i].type == 0u )
		{	
			vec3 toLight = lightSources[i].position.xyz - rasterizerData.position.xyz;
			lightFalloff = 1.0 / length( toLight ); lightFalloff *= lightFalloff; lightFalloff = min( 1, lightFalloff );
			L = normalize( toLight );
		}
		else if ( lightSources[i].type == 1u )
		{
			L = normalize( -lightSources[i].direction );
		}
		else
		{
			vec3 toLight = lightSources[i].position.xyz - rasterizerData.position.xyz;
			lightFalloff = 1.0 / length( toLight ); lightFalloff *= lightFalloff; lightFalloff = min( 1, lightFalloff );
			L = normalize( toLight );
			vec3 A = normalize( -lightSources[i].direction );
			float cosangle = dot( L, A );
			angularCutoff = ( cosangle > lightSources[i].cosangle ) ? 1.0 : 0.0;
		}

		if (lightFalloff * lightSources[i].intensity < EPSILON)
			continue;
		else if (angularCutoff == 0.0)
			continue;

		vec3 R = reflect( -L, N );	
		float dotNL = max( dot( N, L ), 0.0 );
		float phongTerm = pow( max( dot( R, V ), 0.0 ), shininess );
	
		color += lightSources[i].intensity * lightSources[i].color * lightFalloff * angularCutoff * dotNL * kD;
		if (dotNL > 0)
			color += lightSources[i].intensity * lightSources[i].color * lightFalloff * angularCutoff * phongTerm * kS;
	}
	
	fragmentColor = vec4( clamp( color, vec3( 0.0 ), vec3( 1.0 ) ), 1.0 );
}