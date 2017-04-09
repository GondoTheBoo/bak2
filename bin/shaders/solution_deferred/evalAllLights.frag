#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

const float PI = 3.14159265359;
const int MAX_LIGHTS = 100;
const float MAX_SHININESS = 1000.0;
const float EPSILON = 0.01;

uniform sampler2D textureA;
uniform sampler2D textureB;
uniform sampler2D textureC;
uniform sampler2D textureD;
uniform sampler2D textureE;

uniform mat4 screen2eyeTf;

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

struct FragmentAttribs
{
	vec3 kD;
	vec3 kS;
	vec3 kA;
	float shininess;
	vec4 position;
	vec3 normal;
	int shadingType;
};

FragmentAttribs readGBuffer()
{
	FragmentAttribs result;

	vec4 texA = texelFetch(textureA, ivec2(gl_FragCoord.xy), 0);
	vec4 texB = texelFetch(textureB, ivec2(gl_FragCoord.xy), 0);
	vec4 texC = texelFetch(textureC, ivec2(gl_FragCoord.xy), 0);
	vec4 texD = texelFetch(textureD, ivec2(gl_FragCoord.xy), 0);
	vec4 texE = texelFetch(textureE, ivec2(gl_FragCoord.xy), 0);

	result.kD = texC.rgb;
	result.kS = texD.rgb;
	result.kA = texE.rgb;
	result.shininess = texD.a * MAX_SHININESS;
	result.normal = 2*texB.rgb - vec3( 1.0 );

	vec4 screen = vec4(gl_FragCoord.xy, texA.r, 1);
	vec4 eye = screen2eyeTf * screen;
	eye.xyzw /= eye.w;
	result.position = eye;
	result.shadingType = (texC.a == 1.0) ? 1 : 0;

	return result;
}

void main()
{	
	FragmentAttribs attributes = readGBuffer();

	if (attributes.shadingType == 1)
	{
		fragmentColor = vec4( attributes.kD, 1.0 );
	}
	else
	{
		vec3 N = normalize( attributes.normal.xyz );
		vec3 V = normalize( -attributes.position.xyz );

		vec3 ambient = ambientIntensity * ambientColor * attributes.kA;
		vec3 color = ambient;
		for (int i=0; i<min(lightCount, MAX_LIGHTS); ++i)
		{
			float lightFalloff = 1.0;
			float angularCutoff = 1.0;
			vec3 L = vec3( 0.0 );
			if ( lightSources[i].type == 0u )
			{	
				vec3 toLight = lightSources[i].position.xyz - attributes.position.xyz;
				lightFalloff = 1.0 / length( toLight ); lightFalloff *= lightFalloff; lightFalloff = min( 1, lightFalloff );
				L = normalize( toLight );
			}
			else if ( lightSources[i].type == 1u )
			{
				L = normalize( -lightSources[i].direction );
			}
			else
			{
				vec3 toLight = lightSources[i].position.xyz - attributes.position.xyz;
				lightFalloff = 1.0 / length( toLight ); lightFalloff *= lightFalloff; lightFalloff = min( 1, lightFalloff );
				L = normalize( toLight );
				vec3 A = normalize( -lightSources[i].direction );
				float cosangle = dot( L, A );
				angularCutoff = ( cosangle > lightSources[i].cosangle ) ? 1.0 : 0.0;
			}


			if (lightFalloff * lightSources[i].intensity < EPSILON)
				continue;
			if (angularCutoff == 0.0)
				continue;

			vec3 R = reflect( -L, N );	
			float dotNL = max( dot( N, L ), 0.0 );
			float phongTerm = pow( max( dot( R, V ), 0.0 ), attributes.shininess );
	
			color += lightSources[i].intensity * lightSources[i].color * lightFalloff * angularCutoff * dotNL * attributes.kD;
			if (dotNL > 0)
				color += lightSources[i].intensity * lightSources[i].color * lightFalloff * angularCutoff * phongTerm * attributes.kS;
		}
	
		fragmentColor = vec4( clamp( color, vec3( 0.0 ), vec3( 1.0 ) ), 1.0 );
	}
}