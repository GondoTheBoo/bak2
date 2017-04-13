#version 330 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 mvpTf;
uniform mat4 depthBiasMVP;
uniform mat4 viewport_RSM;
uniform mat4 projection_RSM;
uniform mat4 view_RSM;

uniform sampler2D tex_rsm_depth;

out vec4 RsmTexCoor;
  
void main()
{
	vec2 sameplPixel = vec2(600, 300);
	vec4 texDepth = texelFetch(tex_rsm_depth, ivec2(sameplPixel.xy), 0);
	
	vec4 screenPos = vec4( sameplPixel.x, sameplPixel.y, texDepth.r, 1);
	worldPos = inverse( viewport_RSM * projection_RSM * view_RSM ) * screenPos;
	worldPos.xyzw /= worldPos.w;
	
	RsmTexCoor = depthBiasMVP * vec4(vertexPosition, 1.0);
	gl_Position = (mTf * vec4(vertexPosition, 1.0)) + vec4(worldPos.xyz, 0);
}
