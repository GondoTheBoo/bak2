#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

uniform sampler2D textureA;
uniform sampler2D textureB;
uniform sampler2D textureC;
uniform sampler2D textureD;

uniform mat4 screen2eyeTf;

const float EPSILON = 0.01;

in vec4 eyeLightSourcePos;
in vec3 eyeNormal;
in float intensity;

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
	vec4 texD = texelFetch(textureD, ivec2(gl_FragCoord.xy), 0);

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

	//normal at position from lightview in eyespace
	//vec3 N = normalize(eyeNormal);
	//normal at same position but from gBuffer view
	vec3 N = normalize( attributes.normal.xyz );
	vec3 V = normalize( -attributes.position.xyz );

	vec3 toLight = eyeLightSourcePos.xyz - attributes.position.xyz;
	float lightFalloff = 1.0 / length( toLight ); 
	lightFalloff *= lightFalloff; 
	lightFalloff = min( 1, lightFalloff );
	
	vec3 L = normalize( toLight );

	if (lightFalloff * intensity < EPSILON)
		lightFalloff = 0.0;

	float dotNL = max( dot( N, L ), 0.0 );

	vec3 color = intensity * lightFalloff * dotNL * attributes.kD;
	//fragmentColor = vec4( clamp( color, vec3( 0.0 ), vec3( 1.0 ) ), 1.0 );
	fragmentColor = vec4(1.0,1.0,1.0,1.0);
}