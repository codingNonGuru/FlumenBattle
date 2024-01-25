#version 320 es

precision highp float;

layout (location = 1) uniform float weatherFactor;

// TEXTURES

layout (location = 0) out vec4 fragment;

in float temperature;

void main()
{	
	fragment = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	float factor = 0.0f;
	if(temperature + weatherFactor < 0.45f) 
	{
		factor = 1.0f;
	}
	else if(temperature + weatherFactor < 0.5f)
	{
		float t = (temperature + weatherFactor) - 0.45f;
		t *= 20.0f;

		factor = 3.0f * t * t - 2.0f * t * t * t;
		factor = 1.0f - factor;
	}

	fragment.a *= factor;
}
