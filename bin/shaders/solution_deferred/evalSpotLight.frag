#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;


const float PI = 3.14159265359;
const int MAX_LIGHTS = 100;
const float MAX_SHININESS = 1000.0;
const float EPSILON = 0.01;

uniform sampler2D texDepth;
uniform sampler2D texNormal;
uniform sampler2D texDiff;
uniform sampler2D texSpec;

uniform sampler2D tex_rsm_depth;
uniform sampler2D tex_rsm_normal;
uniform sampler2D tex_rsm_diff;
uniform sampler2D tex_rsm_worldCoord;

uniform mat4 screen2eyeTf;
uniform mat4 eye2worldTf;
uniform mat4 lightViewProjectionTf;

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

	vec4 texDepth = texelFetch(texDepth, ivec2(gl_FragCoord.xy), 0);
	vec4 texNormal = texelFetch(texNormal, ivec2(gl_FragCoord.xy), 0);
	vec4 texDiff = texelFetch(texDiff, ivec2(gl_FragCoord.xy), 0);
	vec4 texSpec = texelFetch(texSpec, ivec2(gl_FragCoord.xy), 0);

	result.kD = texDiff.rgb;
	result.kS = texSpec.rgb;
	result.shininess = texSpec.a * MAX_SHININESS;
	result.normal = 2*texNormal.rgb - vec3( 1.0 );

	vec4 screen = vec4(gl_FragCoord.xy, texDepth.r, 1);
	vec4 eye = screen2eyeTf * screen;
	eye.xyzw /= eye.w;
	result.position = eye;
	result.solidFlag = (texDiff.a == 1.0) ? 1 : 0;
	return result;
}

void main()
{	
	FragmentAttribs attributes = readGBuffer();
	
	vec4 worldPos = eye2worldTf * attributes.position;
	vec4 lightEyePos = lightViewProjectionTf * worldPos;
	vec4 lightscreenPos = lightEyePos.xyzw / lightEyePos.w;
	vec4 rsmDepth = texelFetch(tex_rsm_depth,ivec2(lightscreenPos.xy),0);
	
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
			
		float bias = 0.0005;
		float visibility = 1.0;
		if(rsmDepth.r < lightscreenPos.z-bias)
			visibility = 0.5;

		fragmentColor = vec4( clamp( color * visibility, vec3( 0.0 ), vec3( 1.0 ) ), 1.0 );
	}
}