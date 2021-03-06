#version 330

uniform sampler2D tInputTexture;
uniform vec2 v2Scale;

in vec2 v2f_v2TexCoords;

out vec4 color;

void main(void)
{
	color = vec4(0.0f, 0.0f, 0.0f, 0.0f);

	color += 0.25f * texture(tInputTexture, v2f_v2TexCoords);

	color += 0.125f * texture(tInputTexture, vec2(v2f_v2TexCoords.x, v2f_v2TexCoords.y + v2Scale.y));
	color += 0.125f * texture(tInputTexture, vec2(v2f_v2TexCoords.x, v2f_v2TexCoords.y - v2Scale.y));
	color += 0.125f * texture(tInputTexture, vec2(v2f_v2TexCoords.x + v2Scale.x, v2f_v2TexCoords.y));
	color += 0.125f * texture(tInputTexture, vec2(v2f_v2TexCoords.x - v2Scale.x, v2f_v2TexCoords.y));

	color += 0.0625f * texture(tInputTexture, vec2(v2f_v2TexCoords.x + v2Scale.x, v2f_v2TexCoords.y + v2Scale.y));
	color += 0.0625f * texture(tInputTexture, vec2(v2f_v2TexCoords.x + v2Scale.x, v2f_v2TexCoords.y - v2Scale.y));
	color += 0.0625f * texture(tInputTexture, vec2(v2f_v2TexCoords.x - v2Scale.x, v2f_v2TexCoords.y + v2Scale.y));
	color += 0.0625f * texture(tInputTexture, vec2(v2f_v2TexCoords.x - v2Scale.x, v2f_v2TexCoords.y - v2Scale.y));

	//color += 0.0545f * texture(tInputTexture, v2f_v2TexCoords - v2Scale * 2.0f);
	//color += 0.2442f * texture(tInputTexture, v2f_v2TexCoords - v2Scale);
	//color += 0.4026f * texture(tInputTexture, v2f_v2TexCoords);
	//color += 0.2442f * texture(tInputTexture, v2f_v2TexCoords + v2Scale);
	//color += 0.0545f * texture(tInputTexture, v2f_v2TexCoords + v2Scale * 2.0f);

}
