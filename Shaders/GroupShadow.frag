#version 320 es

precision highp float;

layout (location = 4) uniform float opacity;

// TEXTURES

uniform sampler2D diffuse;

layout (location = 0) out vec4 fragment;

in vec2 texCoords;

void main()
{	
	fragment.rgba = texture(diffuse, texCoords).rgba;

    fragment.rgb = vec3(0.0f);

    fragment.a *= 0.6f;
}