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

uniform LightSource lightSource;

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
	result.kS = texD.rgb;
	result.shininess = texD.a * MAX_SHININESS;
	result.normal = 2*texB.rgb - vec3( 1.0 );

	vec4 screen = vec4(gl_FragCoord.xy, texA.r, 1);
	vec4 eye = screen2eyeTf * screen;
	eye.xyzw /= eye.w;
	result.position = eye;
	result.solidFlag = (texC.a == 1.0) ? 1 : 0;
	return result;
}

void main()
{	
	FragmentAttribs attributes = readGBuffer();

	if (attributes.solidFlag == 1)
	{
		fragmentColor = vec4( 0.0, 0.0, 0.0, 1.0 );
	}
	else
	{

		vec3 N = normalize( attributes.normal.xyz );
		vec3 V = normalize( -attributes.position.xyz );

		vec3 toLight = lightSource.position.xyz - attributes.position.xyz;
		float lightFalloff = 1.0 / length( toLight ); lightFalloff *= lightFalloff; lightFalloff = min( 1, lightFalloff );
		vec3 L = normalize( toLight );
		vec3 A = normalize( -lightSource.direction );
		float cosangle = dot( L, A );
		float angularCutoff = ( cosangle > lightSource.cosangle ) ? 1.0 : 0.0;

		if (lightFalloff * lightSource.intensity < EPSILON)
			lightFalloff = 0.0;

		vec3 R = reflect( -L, N );	
		float dotNL = max( dot( N, L ), 0.0 );
		float phongTerm = pow( max( dot( R, V ), 0.0 ), attributes.shininess );
	
		vec3 color = lightSource.intensity * lightSource.color * lightFalloff * angularCutoff * dotNL * attributes.kD;
		if (dotNL > 0)
			color += lightSource.intensity * lightSource.color * lightFalloff * angularCutoff * phongTerm * attributes.kS;
		//fragmentColor = vec4(N,1.0);
		fragmentColor = vec4( clamp( color, vec3( 0.0 ), vec3( 1.0 ) ), 1.0 );
	}
}