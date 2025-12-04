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

layout (std430, binding = 4) buffer TYPES
{
	int types[];	
};

// TEXTURES

// OUTPUT

out vec4 color;

vec2 vertices[5] = vec2[5] 
(
	vec2(0.0f, -0.6f), 
	vec2(-0.166f, -0.4f),
	vec2(0.166f, -0.4f),
	vec2(0.5f, 0.0f), 
	vec2(-0.5f, 0.0f)
);

uint indices[9] = uint[9]
(
	0, 1, 2,
	1, 2, 3,
	1, 3, 4
);

void main()
{
	int indicesPerTile = 9 * int(maxMountainCount);

	uint instanceIndex = uint(gl_VertexID / indicesPerTile);

	uint tileIndex = tileQueue[instanceIndex];

	uint objectIndex = tileIndex * maxMountainCount + uint((gl_VertexID % indicesPerTile) / 9);
	
	uint vertexIndex = uint(gl_VertexID % 9);

    vec2 basePosition = positions[objectIndex];

	vec2 position = vertices[indices[vertexIndex]] * defaultSize * scales[objectIndex] + basePosition;

	gl_Position = viewMatrix * vec4(position.x, position.y, depth + basePosition.y * 0.00001f, 1.0f);

	int typeIndex = types[objectIndex];

	bool isPeak = vertexIndex == uint(0) || vertexIndex == uint(1) || vertexIndex == uint(2);
	bool isHigh = typeIndex == 1;
	
	if(isHigh == true && isPeak == true)
		color = vec4(1.0f);
	else
		color = colors[objectIndex];
}