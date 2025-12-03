#version 320 es

precision highp float;

// TEXTURES

layout (location = 0) out vec4 fragment;

in vec4 color;

void main()
{	
	fragment = color;
	
	fragment.a = 1.0f;
}
