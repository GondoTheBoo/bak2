#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

const float PI = 3.14159265359;
const float MAX_SHININESS = 1000.0;

uniform sampler2D textureA;
uniform sampler2D textureB;
uniform sampler2D textureC;
uniform sampler2D textureD;
uniform sampler2D textureE;

uniform mat4 screen2eyeTf;

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
		vec3 ambient = ambientIntensity * ambientColor * attributes.kA;
		vec3 color = ambient;

		fragmentColor = vec4(color, 1.0 );
	}
}