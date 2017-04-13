#version 330 core

layout (location = 0) in vec3 vertexPosition;

uniform mat4 mvpTf;
  
void main()
{
	gl_Position = mvpTf * vec4(vertexPosition, 1.0);
}
