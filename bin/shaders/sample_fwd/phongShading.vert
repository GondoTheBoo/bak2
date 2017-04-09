#version 330 core

layout ( location = 0 ) in vec3 vertexPosition;
layout ( location = 1 ) in vec3 vertexNormal;
layout ( location = 4 ) in vec2 vertexTexcoord;

uniform mat4 modelviewTf;
uniform mat4 mvpTf;
uniform mat3 normals2eyeTf;

struct RasterizerData
{
	vec4 position;
	vec3 normal;
	vec2 uv;
};

out RasterizerData rasterizerData;

void main()
{
	rasterizerData.position = modelviewTf * vec4( vertexPosition, 1 );
	rasterizerData.normal = normalize( normals2eyeTf * vertexNormal );
	rasterizerData.uv = vertexTexcoord;

	gl_Position = mvpTf * vec4( vertexPosition, 1 );
}