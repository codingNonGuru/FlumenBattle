#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

// TEXTURES

uniform sampler2D diffuse; 

layout (location = 0) out vec4 fragment;

in vec2 textureCoordinates;

void main()
{	
	fragment = vec4(1.0f);//texture(diffuse, textureCoordinates).rgba;
	
	//fragment.a *= opacity;
}
