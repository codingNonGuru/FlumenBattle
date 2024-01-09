#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

// TEXTURES

uniform sampler2D diffuse; 

layout (location = 6) uniform vec4 color;

layout (location = 7) uniform int hasColor;

layout (location = 0) out vec4 fragment;

in vec2 textureCoordinates;

void main()
{	
	fragment = texture(diffuse, textureCoordinates).rgba;

	if(hasColor == 1)
	{
		fragment.rgb = color.rgb;
	}
	
	fragment.a *= opacity;
}
