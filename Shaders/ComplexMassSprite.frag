#version 320 es

precision highp float;

layout (location = 5) uniform int hasOpacity;

// TEXTURES

uniform sampler2D diffuse; 

layout (location = 0) out vec4 fragment;

in vec2 textureCoordinates;

in float opacity;

void main()
{	
	fragment = texture(diffuse, textureCoordinates).rgba;
	
	if(hasOpacity == 1)
	{
		fragment.a *= opacity;
	}
}
