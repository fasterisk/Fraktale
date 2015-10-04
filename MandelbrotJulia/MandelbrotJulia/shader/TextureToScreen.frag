#version 330

uniform sampler2D tInputTexture;

in vec2 v2f_v2TexCoords;

out vec4 color;

void main(void)
{
	color = texture(tInputTexture, v2f_v2TexCoords);
}
