#version 320 es

precision highp float;

layout (location = 1) uniform float weatherFactor;

// TEXTURES

uniform sampler2D diffuse; 

uniform sampler2D diffuse2;

uniform sampler2D diffuseShrunk;

uniform sampler2D diffuseShrunk2;

layout (location = 0) out vec4 fragment;

in float temperature;

in vec2 texCoords;

in float texIndex;

void main()
{	
	fragment = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	float factor = 0.0f;
	if(temperature + weatherFactor < 0.3f) 
	{
		factor = 1.0f;
	}
	else if(temperature + weatherFactor < 0.4f)
	{
		vec4 pixel = texIndex < 0.5f ? texture(diffuse, texCoords).rgba : texture(diffuse2, texCoords).rgba;;

		factor = pixel.a;
	}
	else if(temperature + weatherFactor < 0.5f)
	{
		vec4 pixel = texIndex < 0.5f ? texture(diffuseShrunk, texCoords).rgba : texture(diffuseShrunk2, texCoords).rgba;

		factor = pixel.a;
	}

	fragment.a *= factor;
}
