#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform uint maxMountainCount;

layout (location = 2) uniform float defaultSize;

layout (location = 4) uniform float depth;  

// DATA BUFFERS

layout (std430, binding = 0) buffer POSITIONS
{
	vec2 positions[];	
};

layout (std430, binding = 1) buffer COLORS
{
	vec4 colors[];	
};

layout (std430, binding = 2) buffer SCALES
{
	float scales[];	
};

layout (std430, binding = 3) buffer TILE_QUEUE
{
	uint tileQueue[];	
};

// TEXTURES

// OUTPUT

out vec4 color;

vec2 vertices[6] = vec2[6] (
	vec2(-0.3f, -0.5f), 
	vec2(0.3f, -0.5f), 
	vec2(0.5f, 0.5f), 
	vec2(-0.3f, -0.5f), 
	vec2(0.5f, 0.5f), 
	vec2(-0.5f, 0.5f)
	);

void main()
{
	int verticesPerTile = 6 * int(maxMountainCount);

	uint instanceIndex = uint(gl_VertexID / verticesPerTile);

	uint tileIndex = tileQueue[instanceIndex];

	uint objectIndex = tileIndex * maxMountainCount + uint((gl_VertexID % verticesPerTile) / 6);
	
	uint vertexIndex = uint(gl_VertexID % 6);

    vec2 basePosition = positions[objectIndex];

	vec2 position = vertices[vertexIndex] * defaultSize * scales[objectIndex] + basePosition;

	gl_Position = viewMatrix * vec4(position.x, position.y, depth + basePosition.y * 0.00001f, 1.0f);

	color = colors[objectIndex];
}