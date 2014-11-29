#version 330

uniform sampler2D tRasterTexture;
uniform sampler2D tStartingRegionTexture;

in vec2 v2f_v2TexCoords;

void main(void)
{
	vec4 vColor = vec4(texture(tRasterTexture, v2f_v2TexCoords.st).rgb, 1.0f);
	
	float fStartingRegionVal = texture(tStartingRegionTexture, v2f_v2TexCoords.st).r;
	vColor += fStartingRegionVal;
	vColor.w = 1.0f;

	gl_FragColor = vColor;

}