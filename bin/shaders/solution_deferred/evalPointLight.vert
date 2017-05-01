#version 330 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 vTf;
uniform mat4 vpTf;
uniform mat4 lightScreen2WorldTf;
uniform sampler2D tex_rsm_depth;
uniform sampler2D tex_rsm_normal;
uniform sampler2D tex_rsm_intensity;

uniform mat3 normals2EyeTf;

out vec4 lightEyePos;
out vec3 lightFlux;
out vec3 lightDirectionEye;

void main()
{
//16 values on x axis, 12 values on y axis.
//vec2 sampleXY = vec2((gl_InstanceID%16) * 64, (gl_InstanceID/16) * 64);

//32 values on x and 24 y axis
vec2 sampleXY = vec2((gl_InstanceID%32) * 32, (gl_InstanceID/32) * 32);

//64 values on x and 48 y axis
//vec2 sampleXY = vec2((gl_InstanceID%64) * 16, (gl_InstanceID/64) * 16);

//128 values on x and 96 y axis
//vec2 sampleXY = vec2((gl_InstanceID%128) * 8, (gl_InstanceID/128) * 8);

vec4 texDepth = texelFetch(tex_rsm_depth, ivec2(sampleXY.xy), 0);
vec3 texNormal = texelFetch(tex_rsm_normal, ivec2(sampleXY.xy),0).rgb;
vec3 intensityValues = texelFetch(tex_rsm_intensity, ivec2(sampleXY.xy), 0).rgb;
float scale = 1.0;


vec4 screen = vec4(sampleXY.xy, texDepth.z, 1.0);
vec4 world = lightScreen2WorldTf * screen;
vec3 lightPosition = world.xyz / world.w;
lightPosition = lightPosition - vec3(0.05) * texNormal;
vec3 lightVertex = lightPosition + scale * vertexPosition; 

lightDirectionEye = normalize(normals2EyeTf * texNormal);

//vpl light source info
lightEyePos = vTf * vec4(lightPosition, 1.0);
lightFlux = intensityValues/5000;
//maybe add dotNL

//sphere vertex
gl_Position = vpTf * vec4(lightVertex, 1.0);
}