#version 330
precision highp float;

uniform float fMaxIterations;
uniform float fScale;
uniform vec2 v2Offset;
uniform sampler1D tLookupTable;

in vec2 v2f_v2C;

out vec4 color;

void main(void)
{
	vec2 c = v2f_v2C.xy;
	c = c * fScale + v2Offset;

	vec2 z = vec2(0.0, 0.0);

	color = vec4(0.0, 0.0, 0.0, 1.0);
	for (float i = 0; i < fMaxIterations; i += 1.0)
	{
		if (dot(z, z) > 4.0)
		{
			color = texture(tLookupTable, i / fMaxIterations);
			break;
		}

		z = vec2(z.x*z.x - z.y*z.y, 2.0*z.x*z.y) + c;
	}
}
