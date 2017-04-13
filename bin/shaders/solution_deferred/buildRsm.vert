#version 330 core

layout ( location = 0 ) in vec3 vertexPosition;
layout ( location = 1 ) in vec3 vertexNormal;

uniform mat4 mTf;
uniform mat4 mvpTf;

out vec3 worldSpaceNormal;;
out vec4 worldSpacePos;

void main()
{
	worldSpaceNormal = vec3(normalize(mTf * vec4(vertexNormal,1.0)));
	worldSpacePos = mTf * vec4(vertexPosition,1);

	gl_Position = mvpTf * vec4( vertexPosition, 1 );
}