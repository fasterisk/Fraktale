#version 330
precision highp float;

layout(location=0) in vec3 in_v3Vertex;

smooth out vec2 v2f_v2C;


void main(void)
{
	v2f_v2C.x = in_v3Vertex.x;
	v2f_v2C.y = in_v3Vertex.y;
	

	gl_Position = vec4(in_v3Vertex, 1.0f);
}