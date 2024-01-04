#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

// TEXTURES

uniform sampler2D diffuse; 

layout (location = 5) uniform int hasTexture;

layout (location = 6) uniform vec2 textureOffset;

layout (location = 8) uniform vec4 color;

layout (location = 0) out vec4 fragment;

in vec2 textureCoordinates;

void main()
{	
	if(hasTexture == 1)
	{
		fragment = texture(diffuse, textureCoordinates).rgba;
	}
	else
	{
		fragment = color; //vec4(1.0f);
	}
	
	fragment.a *= opacity;
}
