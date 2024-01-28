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

layout (std430, binding = 3) buffer FLIP_STATES
{
	int isFlipped[];	
};

// TEXTURES

uniform sampler2D diffuse; 

// OUTPUT

out vec2 textureCoordinates;

out float opacity;

void main()
{	
	vec2 vertices[4] = vec2[4](
		vec2(-0.5f, -0.5f), 
		vec2(0.5f, -0.5f), 
		vec2(0.5f, 0.5f), 
		vec2(-0.5f, 0.5f));

	uint indices[6] = uint[6](0, 1, 2, 0, 2, 3);

	uint flippedIndices[6] = uint[6](1, 0, 3, 1, 3, 2);

	uint vertexIndex = uint(gl_VertexID % 6);
	uint objectIndex = uint(gl_VertexID / 6);
	
	uint index = isFlipped[objectIndex] == 1 ? flippedIndices[vertexIndex] : indices[vertexIndex];
	textureCoordinates = vertices[index] + vec2(0.5f, 0.5f);

	textureCoordinates *= textureScale;

	textureCoordinates += textureOffsets[objectIndex];

	ivec2 size = textureSize(diffuse, 0);
	vec2 position = vertices[indices[vertexIndex]] * vec2(float(size.x), float(size.y)) * textureScale * spriteSize + positions[objectIndex];

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);

	opacity = opacities[objectIndex];
}