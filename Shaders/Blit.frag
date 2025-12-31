#version 320 es

precision highp float;

// TEXTURES

uniform sampler2D picture;

layout (location = 0) out vec4 fragment;

in vec2 coords;

void main()
{	
	fragment.rgba = texture(picture, coords).rgba;
}