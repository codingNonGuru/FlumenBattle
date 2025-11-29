#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

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

// TEXTURES

// OUTPUT

void main()
{	
	vec2 vertices[6] = vec2[6] (
        vec2(-0.4f, -0.3f), 
        vec2(0.5f, -0.3f), 
        vec2(0.5f, 0.3f), 
        vec2(-0.4f, -0.3f), 
        vec2(0.5f, 0.3f), 
        vec2(-0.5f, 0.3f)
        );
	
	uint vertexIndex = uint(gl_VertexID % 6);
	uint objectIndex = uint(gl_VertexID / 6);

    vec2 basePosition = positions[objectIndex];

	vec2 position = vertices[vertexIndex] * scales[objectIndex] * 15.0f + basePosition + vec2(3.0f, 4.0f);

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);
}