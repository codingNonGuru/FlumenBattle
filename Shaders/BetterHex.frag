#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

// TEXTURES

layout (location = 0) out vec4 fragment;

//in vec2 textureCoordinates;

in vec4 color;

void main()
{	
	fragment = color; //color;//texture(diffuse, textureCoordinates).rgba;
	
	fragment.a *= opacity;
}
