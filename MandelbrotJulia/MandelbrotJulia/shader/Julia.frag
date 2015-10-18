#version 330

uniform float fMaxIterations;
uniform float fScale;
uniform vec2 v2Offset;
uniform sampler1D tLookupTable;
uniform vec2 v2Complex;
uniform float fTFPar1;
uniform float fTFPar2;

in vec2 v2f_v2TexCoords;

out vec4 color;

void main(void)
{
	vec2 z = v2f_v2TexCoords.xy;
	z = z * fScale + v2Offset;

	vec2 c = v2Complex;

	color = vec4(0.0, 0.0, 0.0, 1.0);
	float i = 0;

	for (; i < fMaxIterations; i += 1.0)
	{
		if (dot(z, z) > max(dot(c,c), 4.0))
		{
			break;
		}

		z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
	}

	if(i < fMaxIterations)
		color = texture(tLookupTable, fTFPar2 + (i / fTFPar1));

}
