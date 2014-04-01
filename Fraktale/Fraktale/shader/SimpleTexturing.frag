#version 330

uniform sampler2D tRasterTexture;
uniform sampler2D tStartingRegionTexture;

in vec2 v2f_v2TexCoords;

void main(void)
{
	gl_FragColor = vec4(texture(tRasterTexture, v2f_v2TexCoords.st).r, texture(tStartingRegionTexture, v2f_v2TexCoords.st).r, 0.0f, 1.0f);
}