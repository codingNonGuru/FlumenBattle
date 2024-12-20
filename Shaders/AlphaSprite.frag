#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

// TEXTURES

uniform sampler2D diffuse; 

layout (location = 8) uniform vec4 color;

layout (location = 0) out vec4 fragment;

in vec2 textureCoordinates;

void main()
{	
    fragment.rgb = color.rgb; 
    fragment.a = texture(diffuse, textureCoordinates).a;
	
	fragment.a *= opacity;
}
