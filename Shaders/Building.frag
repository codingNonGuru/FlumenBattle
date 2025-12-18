#version 320 es

precision highp float;

layout (location = 4) uniform float opacity;

// TEXTURES

layout (location = 0) out vec4 fragment;

in vec3 color;

void main()
{	
	fragment.rgb = color.rgb; //texture(diffuse, textureCoordinates).rgba;
	
	fragment.a = opacity;
}