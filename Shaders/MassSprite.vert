#version 320 es

struct Float3
{
	float x, y, z;
};

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 2) uniform float spriteSize;

layout (location = 4) uniform float depth;  

// DATA BUFFERS

layout (std430, binding = 0) buffer POSITIONS
{
	vec2 positions[];	
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

	vec2 position = vertices[vertexIndex] * spriteSize + positions[objectIndex];

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);
}