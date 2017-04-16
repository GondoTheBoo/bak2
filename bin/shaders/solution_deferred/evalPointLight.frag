#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

uniform sampler2D textureA;
uniform sampler2D textureB;
uniform sampler2D textureC;
//uniform sampler2D textureD;

uniform mat4 screen2eyeTf;

const float EPSILON = 0.01;

in vec4 lightEyePos;
in float lightIntensity;

struct FragmentAttribs
{
	vec3 kD;
	vec3 kS;
	float shininess;
	vec4 position;
	vec3 normal;
	int solidFlag;
};

FragmentAttribs readGBuffer()
{
	FragmentAttribs result;

	vec4 texA = texelFetch(textureA, ivec2(gl_FragCoord.xy), 0);
	vec4 texB = texelFetch(textureB, ivec2(gl_FragCoord.xy), 0);
	vec4 texC = texelFetch(textureC, ivec2(gl_FragCoord.xy), 0);
	
	result.kD = texC.rgb;
	result.normal = 2*texB.rgb - vec3( 1.0 );

	vec4 screen = vec4(gl_FragCoord.xy, texA.r, 1);
	vec4 eye = screen2eyeTf * screen;
	eye.xyzw /= eye.w;
	result.position = eye;
	return result;
}

void main()
{	
	FragmentAttribs attributes = readGBuffer();

	vec3 N = normalize( attributes.normal.xyz );
	vec3 V = normalize( -attributes.position.xyz );

	vec3 toLight = lightEyePos.xyz - attributes.position.xyz;
	float lightFalloff = 1.0 / length( toLight ); lightFalloff *= lightFalloff;	lightFalloff = min( 1, lightFalloff );
	
	vec3 L = normalize( toLight );

	if (lightFalloff * lightIntensity < EPSILON)
		discard;

	float dotNL = max( dot( N, L ), 0.0 );

	vec3 color = lightIntensity * lightFalloff * dotNL * attributes.kD;
	fragmentColor = vec4( clamp( color, vec3( 0.0 ), vec3( 1.0 ) ), 1.0 );
	//fragmentColor = vec4(1.0,1.0,1.0,1.0);
}