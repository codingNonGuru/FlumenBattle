#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

layout (location = 5) uniform vec4 color;

layout (location = 6) uniform float innerSize;

// TEXTURES

layout (location = 0) out vec4 fragment;

//in vec2 textureCoordinates;

in float weight;

void main()
{	
	fragment = color;//texture(diffuse, textureCoordinates).rgba;

	fragment.a *= opacity * (weight > innerSize ? 1.0f : 0.0f);
}
