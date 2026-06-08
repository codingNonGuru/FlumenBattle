#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

layout (location = 5) uniform vec3 color;

layout (location = 6) uniform vec2 heightRange;

layout (location = 7) uniform float distortFactor;

// TEXTURES

uniform sampler2D picture;

uniform sampler2D distort;

layout (location = 0) out vec4 fragment;

in vec2 coords;

in vec2 pos;

void main()
{	
	fragment = texture(picture, coords).rgba;

	float baseFactor = fragment.r;
	
	float distortStrength = texture(distort, pos * 0.0005f).r;

	distortStrength *= baseFactor;

	float height = baseFactor * (1.0f - distortFactor) + distortStrength * distortFactor;

	fragment.a = height >= heightRange.x && height <= heightRange.y ? 1.0f : 0.0f;

	/*if(baseFactor > 0.9f)
		fragment.a = heightRange.y > 0.9f ? 1.0f : 0.0f;*/

	//if(height > 0.65f)
		fragment.rgb = color;
	//else
		//fragment.rgb = vec3(0.95f, 0.85f, 0.7f);
}