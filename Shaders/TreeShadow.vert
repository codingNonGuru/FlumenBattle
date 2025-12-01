#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform uint maxTreeCount;

layout (location = 4) uniform float depth;  

// DATA BUFFERS

layout (std430, binding = 0) buffer POSITIONS
{
	vec2 positions[];	
};

layout (std430, binding = 1) buffer SCALES
{
	float scales[];	
};

layout (std430, binding = 2) buffer TILE_QUEUE
{
	uint tileQueue[];	
};

// TEXTURES

// OUTPUT

vec2 vertices[6] = vec2[6] (
    vec2(-0.4f, -0.3f), 
    vec2(0.5f, -0.3f), 
    vec2(0.5f, 0.3f), 
    vec2(-0.4f, -0.3f), 
    vec2(0.5f, 0.3f), 
    vec2(-0.5f, 0.3f)
    );

void main()
{	
	int verticesPerTile = 6 * int(maxTreeCount);

	uint instanceIndex = uint(gl_VertexID / verticesPerTile);

	uint tileIndex = tileQueue[instanceIndex];

	uint objectIndex = tileIndex * maxTreeCount + uint((gl_VertexID % verticesPerTile) / 6);
	
	uint vertexIndex = uint(gl_VertexID % 6);

    vec2 basePosition = positions[objectIndex];

	vec2 position = vertices[vertexIndex] * scales[objectIndex] * 15.0f + basePosition + vec2(3.0f, 4.0f);

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);
}