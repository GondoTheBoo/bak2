#version 330 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 vTf;
uniform mat4 vpTf;
uniform mat4 lightScreen2WorldTf;
uniform sampler2D tex_rsm_depth;
uniform sampler2D tex_rsm_intensity;


out vec4 lightEyePos;
out float lightIntensity;

void main()
{
float valForMod = gl_InstanceID;
//32 values on x axis.
vec2 sampleXY = vec2((gl_InstanceID%32) * 32, (gl_InstanceID/32) * 32);
//vec2 sampleXY = vec2(0, 0);
vec4 texDepth = texelFetch(tex_rsm_depth, ivec2(sampleXY.xy), 0);
float intensity = texelFetch(tex_rsm_intensity, ivec2(sampleXY.xy), 0).r * 5;

vec4 screen = vec4(sampleXY.xy, texDepth.z, 1.0);
vec4 world = lightScreen2WorldTf * screen;
vec3 lightPosition = world.xyz / world.w;  
vec3 lightVertex = lightPosition + intensity * vertexPosition; 

//vpl light source info
lightEyePos = vTf * vec4(lightPosition, 1.0);
lightIntensity = intensity;
//maybe add dotNL

//sphere vertex
gl_Position = vpTf * vec4(lightVertex, 1.0);
}