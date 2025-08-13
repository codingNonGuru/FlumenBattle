#version 320 es

precision highp float;

layout (location = 5) uniform vec4 color;

layout (location = 6) uniform float opacity;

// TEXTURES

layout (location = 0) out vec4 fragment;

void main()
{	
	fragment = color;
	
	fragment.a *= opacity;
}