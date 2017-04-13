#version 330 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 mTf;
uniform mat4 vpTf;
uniform mat4 lightScreen2WorldTf;
uniform sampler2D tex_rsm_depth;
  
void main()
{
	vec2 sameplPixel = vec2(600, 300);
	vec4 texDepth = texelFetch(tex_rsm_depth, ivec2(sameplPixel.xy), 0);
	
	vec4 screenPos = vec4( sameplPixel.x, sameplPixel.y, texDepth.r, 1);
	vec4 worldPos = lightScreen2WorldTf * screenPos;
	worldPos.xyzw /= worldPos.w;
	
	vec4 vertexWorldPos = mTf * vec4(vertexPosition, 1.0);
	vec4 newWorldPos = vertexWorldPos + vec4(worldPos.xyz, 0);
	
	gl_Position = vpTf * newWorldPos;
}
