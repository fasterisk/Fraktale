#version 330

uniform sampler2D tTexture;

in vec2 v2f_v2TexCoords;

void main(void)
{
	gl_FragColor = vec4(texture(tTexture, v2f_v2TexCoords.st).rgb, 1.0f);;

}