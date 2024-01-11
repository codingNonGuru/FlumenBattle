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

layout (std430, binding = 1) buffer COLORS
{
	vec4 colors[];	
};

// TEXTURES

// OUTPUT

out vec4 color;

void main()
{	
	vec2 vertices[7] = vec2[7](
		vec2(0.0f, 0.0f), 
		vec2(0.866f, 0.5f),
		vec2(0.0f, 1.0f),
		vec2(-0.866f, 0.5f),
		vec2(-0.866f, -0.5f),
		vec2(0.0f, -1.0f),
		vec2(0.866f, -0.5f)
		);

	int indices[18] = int[18](
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 1
	);
	
	uint vertexIndex = uint(indices[gl_VertexID % 18]);
	uint objectIndex = uint(gl_VertexID / 18);

	vec2 position = vertices[vertexIndex] * hexSize + positions[objectIndex];

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);

	color = colors[objectIndex];
}