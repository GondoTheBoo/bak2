#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

uniform sampler2D texDepth;
uniform sampler2D texDiff;
uniform sampler2D texAmb;

uniform mat4 screen2eyeTf;

uniform float ambientIntensity = 0.1;
uniform vec3 ambientColor = vec3( 1.0 );

struct FragmentAttribs
{
	vec3 kD;
	vec3 kA;
	vec4 position;
	float zValue;
	int solidFlag;
};

FragmentAttribs readGBuffer()
{
	FragmentAttribs result;

	vec4 texA = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0);
	vec4 texC = texelFetch(texDiff, ivec2(gl_FragCoord.xy), 0);
	vec4 texE = texelFetch(texAmb, ivec2(gl_FragCoord.xy), 0);

	result.kD = texC.rgb;
	result.kA = texE.rgb;
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