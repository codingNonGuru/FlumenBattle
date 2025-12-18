#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

// TEXTURES

uniform sampler2D distortAngle;

layout (location = 0) out vec4 fragment;

in vec3 color;

void main()
{	
	fragment.rgb = color;
	
	fragment.a = 1.0f;
}