#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 hexPosition;

layout (location = 2) uniform float hexSize;

layout (location = 4) uniform float depth;  

// DATA BUFFERS

// TEXTURES

// OUTPUT

void main()
{	
	vec2 vertices[7] = vec2[7](
		vec2(0.0f, 0.0f), 
		vec2(1.0f, 0.0f), 
		vec2(0.5f, 0.866f), 
		vec2(-0.5f, 0.866f), 
		vec2(-1.0f, 0.0f), 
		vec2(-0.5f, -0.866f), 
		vec2(0.5f, -0.866f)
		);

	int indices[18] = int[18](
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 5,
		0, 5, 6,
		0, 6, 1
	);
	
	uint vertexIndex = uint(indices[gl_VertexID]);

	vec2 position = vertices[vertexIndex] * hexSize + hexPosition;

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);
}