#version 330 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 mTf;
uniform mat4 vTf;
uniform mat4 vpTf;
uniform mat4 lightScreen2WorldTf;
uniform sampler2D tex_rsm_depth;
uniform sampler2D tex_rsm_worldPos;
uniform sampler2D tex_rsm_normal;

out vec4 eyeLightSourcePos;
out vec3 eyeNormal;
  
void main()
{
	vec2 sampleXY = vec2(600, 300);
	vec4 texDepth = texelFetch(tex_rsm_depth, ivec2(sampleXY.xy), 0);
	vec4 lightWorldPos = texelFetch(tex_rsm_worldPos, ivec2(sampleXY.xy), 0);
	vec4 worldNormal = texelFetch(tex_rsm_normal, ivec2(sampleXY.xy),0);
	
	vec4 vertexWorldPos = mTf * vec4(vertexPosition, 1.0);
	eyeLightSourcePos = vTf * lightWorldPos;
	eyeNormal = normalize((vTf * worldNormal).xyz);
	
	vec4 newWorldPos = vertexWorldPos + lightWorldPos;
	gl_Position = vpTf * newWorldPos;
}
