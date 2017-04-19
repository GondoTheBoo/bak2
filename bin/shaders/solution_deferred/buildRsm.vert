#version 330 core

layout ( location = 0 ) in vec3 vertexPosition;
layout ( location = 1 ) in vec3 vertexNormal;
layout ( location = 4 ) in vec2 vertexTexcoord;

uniform mat4 mTf;
uniform mat4 mvpTf;
uniform mat3 normals2WorldTf;

out vec3 eyeSpaceNormal;
out vec3 worldSpaceNormal;
out vec4 worldSpacePos;
out vec2 uv; 

void main()
{
	worldSpaceNormal = normalize(normals2WorldTf * vertexNormal);
	worldSpacePos = mTf * vec4(vertexPosition,1);
	uv = vertexTexcoord;
	gl_Position = mvpTf * vec4( vertexPosition, 1 );
}