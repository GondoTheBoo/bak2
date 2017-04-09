#version 330 core
 
const vec2 position[4] = vec2[]
(
   vec2(-1.0,  1.0),
   vec2(-1.0, -1.0),
   vec2( 1.0,  1.0),
   vec2( 1.0, -1.0)
);
 
void main()
{
	gl_Position = vec4( position[ gl_VertexID ], 0.0, 1.0);
}