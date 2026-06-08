#version 320 es

precision highp float;

layout (location = 5) uniform float scaleFactor;

layout (location = 6) uniform vec2 displaceFactor;

// TEXTURES

uniform sampler2D picture;

uniform sampler2D distort;

layout (location = 0) out vec4 fragment;

in vec2 coords;

in vec2 pos;

void main()
{	
	fragment.rgba = texture(picture, coords).rgba;
	
	float opacityFactor = texture(distort, (pos + displaceFactor) * scaleFactor).r;

	fragment.a = fragment.a * (opacityFactor > 0.5f ? 1.0f : 0.0f);
}