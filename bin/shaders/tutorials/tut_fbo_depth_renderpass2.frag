#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

uniform sampler2D tex;
uniform float zNear;
uniform float zFar;

in vec2 uv;

void main()
{
	float z = texture( tex, uv ).r;
	float linearZ = ( 2 * zNear ) / ( zFar + zNear - z * ( zFar - zNear ) );

	if ( uv.x < 0.5 )
		fragmentColor = vec4( vec3( linearZ ), 1.0 );
	else
		fragmentColor = vec4( vec3( z ), 1.0 );
}