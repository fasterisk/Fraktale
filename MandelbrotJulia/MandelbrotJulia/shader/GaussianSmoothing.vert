#version 330

layout(location=0) in vec3 in_v3Vertex;

smooth out vec2 v2f_v2TexCoords;


void main(void)
{
	v2f_v2TexCoords.x = (in_v3Vertex.x + 1.0f) / 2.0f;
	v2f_v2TexCoords.y = (in_v3Vertex.y + 1.0f) / 2.0f;
	

	gl_Position = vec4(in_v3Vertex, 1.0f);
}