#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

layout (location = 5) uniform vec4 color;

// TEXTURES

layout (location = 0) out vec4 fragment;

//in vec2 textureCoordinates;

in float alpha;

in vec2 coords;

void main()
{	
	fragment = color; //color;//texture(diffuse, textureCoordinates).rgba;
	
	fragment.a = alpha;

    float distance = length(coords);
    float falloff = exp(-distance * distance * 16.0f);
    falloff = pow(falloff, pow(2.0f, 5.0f - 10.0f * falloff));

    falloff = clamp(falloff, 0.0f, 1.0f);

    fragment.a *= falloff;
}
