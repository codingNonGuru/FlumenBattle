#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

// TEXTURES

layout (location = 0) out vec4 fragment;

//in vec2 textureCoordinates;

in float color;

in vec2 coords;

void main()
{	
	fragment = vec4(1.0f); //color;//texture(diffuse, textureCoordinates).rgba;
	
	fragment.a = color;

    float distance = length(coords);
    float falloff = exp(-distance * distance * 7.0f);
    falloff = pow(falloff, pow(2.0f, 6.0f - 12.0f * falloff));

    falloff = clamp(falloff, 0.0f, 1.0f);

    fragment.a *= falloff;
}
