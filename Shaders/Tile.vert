#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 2) uniform float hexSize;

layout (location = 4) uniform float depth;  

// DATA BUFFERS

layout (std430, binding = 0) buffer POSITIONS
{
	vec2 positions[];	
};

layout (std430, binding = 1) buffer RELIEFS
{
	int reliefs[];	
};

layout (std430, binding = 2) buffer TILE_INDICES
{
	int tileIndices[];	
};

// TEXTURES

// OUTPUT

out float color;

out vec2 coords;

void main()
{	
    vec2 vertices[4] = vec2[4] (
        vec2(-0.5f, -0.5f), 
        vec2(0.5f, -0.5f), 
        vec2(0.5f, 0.5f), 
        vec2(-0.5f, 0.5f)
        );

	/*vec2 vertices[7] = vec2[7](
		vec2(0.0f, 0.0f), 
		vec2(0.866f, 0.5f),
		vec2(0.0f, 1.0f),
		vec2(-0.866f, 0.5f),
		vec2(-0.866f, -0.5f),
		vec2(0.0f, -1.0f),
		vec2(0.866f, -0.5f)
		);*/

	uint indices[6] = uint[6](
		0, 1, 2,
		0, 2, 3
		/*0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 1*/
	);
	
	uint vertexIndex = indices[gl_VertexID % 6];
	
	uint objectIndex = uint(tileIndices[gl_VertexID / 6]);

	vec2 position = vertices[vertexIndex] * hexSize + positions[objectIndex];

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);

	color = reliefs[objectIndex] == 1 ? 1.0f : 0.0f;

    coords = vertices[vertexIndex];
}