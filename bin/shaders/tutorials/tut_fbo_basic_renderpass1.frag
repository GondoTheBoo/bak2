#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

in vec3 normal;
in vec4 position;

void main()
{	
	vec3 N = normalize( normal );
	vec3 V = normalize( -position.xyz );
	vec3 L = V;
	vec3 H = normalize( L + V );

	float dotNL = max( dot( N, L ), 0.0 );
	vec3 color = vec3( 0.33, 0.66, 0.99 ) * vec3( dotNL );
	if ( dotNL > 0.0 )
		color += vec3( 0.33, 0.66, 0.99 ) * vec3( pow( max( dot( H, N ), 0.0 ), 4.0 ) );

	color = clamp( color, vec3( 0.0 ), vec3( 1.0 ) );

	fragmentColor = vec4( color, 1.0 );
}