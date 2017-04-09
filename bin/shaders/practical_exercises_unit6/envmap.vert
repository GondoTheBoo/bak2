#version 330 core

layout ( location = 0 ) in vec3 vertexPosition;
layout ( location = 1 ) in vec3 vertexNormal;

uniform mat4 modelTf;
uniform mat4 mvpTf;
uniform mat3 normals2worldTf;

struct RasterizerData
{
	vec4 position;
	vec3 normal;
};

out RasterizerData rasterizerData;

void main()
{
	rasterizerData.position = modelTf * vec4( vertexPosition, 1 );
	rasterizerData.normal = normalize( normals2worldTf * vertexNormal );

	gl_Position = mvpTf * vec4( vertexPosition, 1 );
}