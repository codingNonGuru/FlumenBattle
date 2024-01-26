#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 textureScale;

layout (location = 2) uniform float spriteSize;

layout (location = 4) uniform float depth;  

// DATA BUFFERS

layout (std430, binding = 0) buffer POSITIONS
{
	vec2 positions[];	
};

layout (std430, binding = 1) buffer TEXTURE_OFFSETS
{
	vec2 textureOffsets[];	
};

layout (std430, binding = 2) buffer OPACITIES
{
	float opacities[];	
};

// TEXTURES

uniform sampler2D diffuse; 

// OUTPUT

out vec2 textureCoordinates;

out float opacity;

void main()
{	
	vec2 vertices[6] = vec2[6](
		vec2(-0.5f, -0.5f), 
		vec2(0.5f, -0.5f), 
		vec2(0.5f, 0.5f), 
		vec2(-0.5f, -0.5f), 
		vec2(0.5f, 0.5f), 
		vec2(-0.5f, 0.5f));

	uint vertexIndex = uint(gl_VertexID % 6);
	uint objectIndex = uint(gl_VertexID / 6);
	
	textureCoordinates = vertices[vertexIndex] + vec2(0.5f, 0.5f);

	textureCoordinates *= textureScale;

	textureCoordinates += textureOffsets[objectIndex];

	ivec2 size = textureSize(diffuse, 0);
	vec2 position = vertices[vertexIndex] * vec2(float(size.x), float(size.y)) * textureScale * spriteSize + positions[objectIndex];

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);

	opacity = opacities[objectIndex];
}