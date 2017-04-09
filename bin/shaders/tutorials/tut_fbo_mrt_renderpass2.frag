#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

uniform sampler2D greyscaleTex;
uniform sampler2D colorTex;

uniform vec3 edgeColor;
uniform float edgeThreshold;

in vec2 uv;

vec3 sobelFilter_interpolated( sampler2D tex )
{
	vec2 uvSize = vec2( 1.0 ) / vec2( textureSize( tex, 0 ) );

	vec3 t0 = texture( tex, uv + vec2( -1, -1 ) * uvSize ).rgb;
	vec3 t1 = texture( tex, uv + vec2(  0, -1 ) * uvSize ).rgb;
	vec3 t2 = texture( tex, uv + vec2(  1, -1 ) * uvSize ).rgb;
	vec3 t3 = texture( tex, uv + vec2( -1,  0 ) * uvSize ).rgb;
	vec3 t5 = texture( tex, uv + vec2(  1,  0 ) * uvSize ).rgb;
	vec3 t6 = texture( tex, uv + vec2( -1,  1 ) * uvSize ).rgb;
	vec3 t7 = texture( tex, uv + vec2(  0,  1 ) * uvSize ).rgb;
	vec3 t8 = texture( tex, uv + vec2(  1,  1 ) * uvSize ).rgb;

	vec3 gx = t0 + 2*t1 + t2 - t6 - 2*t7 - t8;
	vec3 gy = t2 + 2*t5 + t8 - t0 - 2*t3 - t6;

	return sqrt( gx*gx + gy*gy );
}

void main()
{
	vec3 g = sobelFilter_interpolated( greyscaleTex );
	if ( g.x > edgeThreshold && g.y > edgeThreshold && g.z > edgeThreshold )
		fragmentColor = vec4( edgeColor, 1.0 );
	else
		fragmentColor = vec4( texture( colorTex, uv ).rgb, 1.0 );
}