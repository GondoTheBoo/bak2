#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

const int SHADING_SOLIDCOLOR = 0;
const int SHADING_LAMBERT = 1;

in vec4 position;
in vec3 normal;

struct SpotLight
{
	sampler2D gobo;
	vec4 position;
	vec3 direction;
	float cosangle;
	vec3 color;
	float intensity;	
};

struct Material
{
	int shadingType;	// 0 == diffuse color | 1 == lambert
	vec3 kDiffuse;
	vec3 kSpecular;
	vec3 kAmbient;
	float shininess;
};

struct Environment
{
	vec3 ambientColor;
	float ambientIntensity;
};

uniform SpotLight light;
uniform Material material;
uniform Environment environment;

uniform vec4 cameraPosition;

void main()
{	
	if (material.shadingType == SHADING_SOLIDCOLOR)
	{
		fragmentColor = vec4( material.kDiffuse, 1 );
	}
	else
	{
		vec3 N = normalize( normal );
		vec3 toLight = light.position.xyz - position.xyz;
		vec3 L = normalize( toLight );
		float dotNL = max( dot( N, L ), 0 );
		float lightFalloff = 1/length(toLight); lightFalloff *= lightFalloff;
		vec3 lightDir = normalize(light.direction);
		float cos_spotlight = dot(lightDir, -L);
		float angularCutoff = (cos_spotlight > light.cosangle) ? 1 : 0;		
		vec3 lightContrib = dotNL * angularCutoff * lightFalloff * light.intensity * light.color;

		// a white low intensity point light is hardcoded @ camera position, so that you can see a bit more of the scene -> leave these 2 lines alone
			vec3 V = normalize(cameraPosition.xyz-position.xyz);
			float dotNLcam = max( dot( N, V ), 0 );

		fragmentColor = vec4( lightContrib * material.kDiffuse + environment.ambientColor * environment.ambientIntensity * material.kAmbient + 0.1 * dotNLcam * material.kDiffuse, 1.0 );
	}
}