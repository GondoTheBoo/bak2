#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

const float PI = 3.14159265359;
const int MAX_LIGHTS = 100;
const float MAX_SHININESS = 1000.0;

uniform sampler2D textureA;
uniform sampler2D textureB;
uniform sampler2D textureC;
uniform sampler2D textureD;
uniform sampler2D textureE;

uniform mat4 screen2eyeTf;

struct LightSource
{
	float intensity;
	vec3 color;
	vec3 direction;
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
	float zValue;
	vec3 normal;
	int solidFlag;
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

	result.zValue = texA.r;

	result.solidFlag = (texC.a == 1.0) ? 1 : 0;

	return result;
}

void main()
{	
	FragmentAttribs attributes = readGBuffer();
	gl_FragDepth = attributes.zValue;

	if (attributes.solidFlag == 1.0)
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
			vec3 L = normalize( -lightSources[i].direction );

			vec3 R = reflect( -L, N );	
			float dotNL = max( dot( N, L ), 0.0 );
			float phongTerm = pow( max( dot( R, V ), 0.0 ), attributes.shininess );
	
			color += lightSources[i].intensity * lightSources[i].color * dotNL * attributes.kD;
			if (dotNL > 0)
				color += lightSources[i].intensity * lightSources[i].color * phongTerm * attributes.kS;
		}
		//fragmentColor = vec4(1.0, 0.0, 0.0, 1.0);
		fragmentColor = vec4( clamp( color, vec3( 0.0 ), vec3( 1.0 ) ), 1.0 );
	}
}