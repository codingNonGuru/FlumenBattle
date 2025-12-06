#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 4) uniform float depth;  

// DATA BUFFERS

layout (std430, binding = 0) buffer POSITIONS
{
	vec2 positions[];	
};

layout (std430, binding = 1) buffer ROTATIONS
{
	float rotations[];	
};

layout (std430, binding = 2) buffer THICKNESSES
{
	float thicknesses[];	
};

layout (std430, binding = 3) buffer LENGTHS
{
	float lengths[];	
};

layout (std430, binding = 4) buffer COLORS
{
	vec4 colors[];	
};

// TEXTURES

// OUTPUT

out vec4 color;

void main()
{	
    uint objectIndex = uint(gl_VertexID / 6);

    float rotation = rotations[objectIndex];

    vec2 vertices[6] = vec2[6] (
        vec2(-0.5f, -0.5f), 
        vec2(0.5f, -0.5f), 
        vec2(0.5f, 0.5f), 
        vec2(-0.5f, -0.5f), 
        vec2(0.5f, 0.5f), 
        vec2(-0.5f, 0.5f)
        );

    float s = sin(rotation);
    float c = cos(rotation);

    vec2 position = vertices[gl_VertexID % 6];
    
    position = vec2(position.x * lengths[objectIndex], position.y * thicknesses[objectIndex]);

    float xnew = position.x * c - position.y * s;
    float ynew = position.x * s + position.y * c;

    position = vec2(xnew, ynew) + positions[objectIndex];

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);

    color = colors[objectIndex];
}