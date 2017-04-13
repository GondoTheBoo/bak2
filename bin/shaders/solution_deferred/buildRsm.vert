#version 330 core

layout ( location = 0 ) in vec3 vertexPosition;
layout ( location = 1 ) in vec3 vertexNormal;
layout ( location = 4 ) in vec2 vertexTexcoord;

uniform mat4 mTf;
uniform mat4 mvpTf;
uniform mat3 normals2eyeTf;


struct RasterizerData
{
	vec3 normal;
	vec2 uv;
};

out RasterizerData rasterizerData;
out vec4 worldSpacePos;

void main()
{
	rasterizerData.normal = normalize( normals2eyeTf * vertexNormal );
	rasterizerData.uv = vertexTexcoord;
	worldSpacePos = mTf * vec4(vertexPosition,1);

	gl_Position = mvpTf * vec4( vertexPosition, 1 );
}