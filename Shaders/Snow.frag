#version 320 es

precision highp float;

layout (location = 1) uniform float weatherFactor;

// TEXTURES

layout (location = 0) out vec4 fragment;

//in vec2 textureCoordinates;

in float temperature;

void main()
{	
	fragment = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	fragment.a *= temperature + weatherFactor > 0.5f ? 0.0f : 1.0f;
}
