#version 330 core

layout ( location = 0 ) out vec4 fragmentColor;

uniform sampler2D tex;
uniform ivec2 viewportDimensions = ivec2( 1024, 1024 );

// the glsl command 'texture' reads the texture value, using a uv coordinate
// uv coords are floats, typically in range[0, 1]
// -> since these do not map 1:1 to texels, what you get is an interpolated value
// how excactly this interolation is performed depends on the filter settings of the texture in question
// in this example, it's linear interpolation

vec3 sobelFilter_interpolated( sampler2D tex )
{
	vec2 uv = gl_FragCoord.xy / viewportDimensions;
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

// the glsl command 'texelFetch' reads the value of a single texel
// no interpolation whatsoever is performed
// as a result, this function does not accept uv coords -
// you need to supply integer pixel coordinates. you can get these e.g. by casting gl_FragCoord.xy to int
// also, since each texture has multiple levels of detail, you need to specify which LOD you want to access
// (this will typically be LOD 0, unless you do something fancy with mipmaps)
// 'texelFetch' naturally is faster that 'texture', and does not support coordinate wrapping

vec3 sobelFilter_exact( sampler2D tex )
{
	ivec2 offset = ( viewportDimensions - textureSize(tex, 0) ) >> 1;
	ivec2 texel = ivec2( gl_FragCoord.xy ) - offset;

	vec3 t0 = texelFetch( tex, texel + ivec2( -1, -1 ), 0 ).rgb;
	vec3 t1 = texelFetch( tex, texel + ivec2(  0, -1 ), 0 ).rgb;
	vec3 t2 = texelFetch( tex, texel + ivec2(  1, -1 ), 0 ).rgb;
	vec3 t3 = texelFetch( tex, texel + ivec2( -1,  0 ), 0 ).rgb;
	vec3 t5 = texelFetch( tex, texel + ivec2(  1,  0 ), 0 ).rgb;
	vec3 t6 = texelFetch( tex, texel + ivec2( -1,  1 ), 0 ).rgb;
	vec3 t7 = texelFetch( tex, texel + ivec2(  0,  1 ), 0 ).rgb;
	vec3 t8 = texelFetch( tex, texel + ivec2(  1,  1 ), 0 ).rgb;

	vec3 gx = t0 + 2*t1 + t2 - t6 - 2*t7 - t8;
	vec3 gy = t2 + 2*t5 + t8 - t0 - 2*t3 - t6;

	return sqrt( gx*gx + gy*gy );
}

void main()
{
	ivec2 textureDimensions = textureSize( tex, 0 );
	if ( textureDimensions.x <= viewportDimensions.x && textureDimensions.y <= viewportDimensions.y )
	{
		fragmentColor = vec4( sobelFilter_exact( tex ), 1.0 );
	}
	else
	{
		fragmentColor = vec4( sobelFilter_interpolated( tex ), 1.0 );
	}	
}