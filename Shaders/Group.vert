#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform float depth;

struct GroupData
{
    vec2 Position;

    int MemberCount;
};

// DATA BUFFERS

layout (std430, binding = 0) buffer GROUP_DATA
{
	GroupData groupData[];	
};

vec2 vertices[6] = vec2[6] (
    vec2(-0.5f, -0.5f), 
    vec2(0.5f, -0.5f), 
    vec2(0.5f, 0.5f), 
    vec2(-0.5f, -0.5f), 
    vec2(0.5f, 0.5f), 
    vec2(-0.5f, 0.5f)
    );

out vec2 texCoords;

void main()
{
    uint groupIndex = uint(gl_VertexID / 6);

    uint vertexIndex = uint(gl_VertexID % 6);

    vec2 position = groupData[groupIndex].Position + vertices[vertexIndex] * 17.0f;

    texCoords = vertices[vertexIndex] + vec2(0.5f, 0.5f);

    gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);
}