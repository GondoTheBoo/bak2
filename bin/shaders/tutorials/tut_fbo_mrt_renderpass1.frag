#version 330 core

layout ( location = 0 ) out vec4 fragmentGoochColor;
layout ( location = 1 ) out vec4 fragmentSolidColor;

in vec3 normal;
in vec4 position;
in vec3 specularColor;
in vec3 diffuseColor;

uniform vec4 lightPosition;
uniform float shininess;
uniform vec3 warmColor = vec3( 0.99, 0.88, 0.33 );
uniform vec3 coolColor = vec3( 0.33, 0.88, 0.99 );
uniform float blendFactor = 0.8;

void main()
{	
	vec3 N = normalize( normal );
	vec3 V = normalize( -position.xyz );
	vec3 L = normalize( lightPosition.xyz - position.xyz );
	vec3 H = normalize( L + V );

	vec3 coolDiff = mix( diffuseColor, coolColor, blendFactor );
	vec3 warmDiff = mix( diffuseColor, warmColor, blendFactor );

	vec3 goochColor = vec3( 0.0 );	

	float dotNL = dot( N, L );
	goochColor += mix( coolDiff, warmDiff,  0.5 + 0.5 * dotNL );
	if ( dotNL > 0.0 )
		goochColor += vec3( pow( max( dot( H, N ), 0.0 ), shininess ) );

	goochColor = clamp( goochColor, vec3( 0.0 ), vec3( 1.0 ) );

	fragmentGoochColor = vec4( goochColor, 1.0 );
	fragmentSolidColor = vec4( vec3( max( dot( N, V ), 0.0 ) ), 1.0 );
}