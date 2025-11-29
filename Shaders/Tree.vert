#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 2) uniform float treeSize;

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

// TEXTURES

// OUTPUT

out vec4 color;

void main()
{	
	vec2 vertices[6] = vec2[6] (
        vec2(-0.3f, -0.5f), 
        vec2(0.3f, -0.5f), 
        vec2(0.5f, 0.5f), 
        vec2(-0.3f, -0.5f), 
        vec2(0.5f, 0.5f), 
        vec2(-0.5f, 0.5f)
        );
	
	uint vertexIndex = uint(gl_VertexID % 6);
	uint objectIndex = uint(gl_VertexID / 6);

    vec2 basePosition = positions[objectIndex];

	vec2 position = vertices[vertexIndex] * treeSize * scales[objectIndex] + basePosition;

	gl_Position = viewMatrix * vec4(position.x, position.y, depth + basePosition.y * 0.00001f, 1.0f);

	color = colors[objectIndex];
}