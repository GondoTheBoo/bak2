#version 330 core
 
const vec2 position[4] = vec2[]
(
   vec2(-1.0,  1.0),
   vec2(-1.0, -1.0),
   vec2( 1.0,  1.0),
   vec2( 1.0, -1.0)
);
 
const vec2 texcoord[4] = vec2[]
(
   vec2(0.0, 1.0),
   vec2(0.0, 0.0),
   vec2(1.0, 1.0),
   vec2(1.0, 0.0)
);
 
out vec2 uv;

void main()
{
	uv = texcoord[ gl_VertexID ];
	gl_Position = vec4( position[ gl_VertexID ], 0.0, 1.0);
}