#version 320 es

precision highp float;

layout (location = 3) uniform float opacity;

// TEXTURES

uniform sampler2D picture;

uniform sampler2D distort;

layout (location = 0) out vec4 fragment;

in vec2 coords;

in vec2 pos;

void main()
{	
	fragment = texture(picture, coords).rgba;

	float baseFactor = clamp(fragment.r, 0.0f, 1.0f);
	
	float distortStrength = texture(distort, pos * 0.002f).r;

	float height = baseFactor * 0.5f + distortStrength * 0.5f;

	fragment.a = height > 0.5f ? 1.0f : 0.0f;

	//if(height > 0.65f)
		fragment.rgb = vec3(0.4f, 0.6f, 0.05f);
	//else
		//fragment.rgb = vec3(0.95f, 0.85f, 0.7f);
}