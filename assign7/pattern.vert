#version 330 compatibility

out vec3 vPosition;

void
main( )
{ 
	vec3 vert = gl_Vertex.xyz;
	vPosition = vert;
	gl_Position = gl_ModelViewProjectionMatrix * vec4( vert, 1. );
}
