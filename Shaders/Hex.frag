#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

layout (location = 5) uniform vec4 color;

// TEXTURES

layout (location = 0) out vec4 fragment;

//in vec2 textureCoordinates;

void main()
{	
	fragment = color;//texture(diffuse, textureCoordinates).rgba;
	
	//fragment.a *= opacity;
}
