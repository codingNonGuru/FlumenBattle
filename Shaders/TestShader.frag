#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

// TEXTURES

uniform sampler2D picture;

layout (location = 0) out vec4 fragment;

in vec2 coords;

void main()
{	
	fragment = texture(picture, coords).rgba;
	
	fragment.a = fragment.r;
}