#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 textureSize;

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

// TEXTURES

// OUTPUT

out vec2 textureCoordinates;

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

	textureCoordinates *= vec2(0.25f, 0.5f);//textureSize;

	textureCoordinates += textureOffsets[objectIndex];

	vec2 position = vertices[vertexIndex] * spriteSize + positions[objectIndex];

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);
}