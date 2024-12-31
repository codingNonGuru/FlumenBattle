#version 320 es

precision highp float;

// TEXTURES

layout (location = 0) out vec4 fragment;

in vec2 pos;

void main()
{
	if((int(pos.x) % 40 < 20 && int(pos.y) % 40 < 20) || (int(pos.x) % 40 >= 20 && int(pos.y) % 40 >= 20))
		fragment = vec4(0.6f, 0.6f, 0.6f, 1.0f);
	else
		fragment = vec4(0.9f, 0.9f, 0.9f, 1.0f);
}
