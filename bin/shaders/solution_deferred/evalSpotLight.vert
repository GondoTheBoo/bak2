#version 330 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 mvpTf;
uniform mat4 local2lightScreenTf;

out vec4 RsmTexCoor;
  
void main()
{
	RsmTexCoor = local2lightScreenTf * vec4(vertexPosition, 1.0);
	gl_Position = mvpTf * vec4(vertexPosition, 1.0);
}
