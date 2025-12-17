#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform float depth;

struct BuildingData
{
    vec4 Color;

    vec2 Position;

    float Rotation;

    float Size;
};

// DATA BUFFERS

layout (std430, binding = 0) buffer BUILDING_DATA
{
	BuildingData buildingData[];	
};

// TEXTURES

// OUTPUT

out vec3 color;

vec2 vertices[6] = vec2[6] (
    vec2(-0.5f, -0.5f), 
    vec2(0.5f, -0.5f), 
    vec2(0.5f, 0.5f), 
    vec2(-0.5f, -0.5f), 
    vec2(0.5f, 0.5f), 
    vec2(-0.5f, 0.5f)
    );

void main()
{
    uint settlementIndex = uint(gl_VertexID / 64);

    uint buildingIndex = uint(gl_VertexID / 6);

    uint vertexId = uint(gl_VertexID % 6);

    float rotation = buildingData[buildingIndex].Rotation;

    float size = buildingData[buildingIndex].Size;

    float s = sin(rotation);
    float c = cos(rotation);

    vec2 position = vertices[vertexId];
    
    position = vec2(position.x * size, position.y * size);

    float xnew = position.x * c - position.y * s;
    float ynew = position.x * s + position.y * c;

    position = vec2(xnew, ynew) + buildingData[buildingIndex].Position;

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);

    color = buildingData[buildingIndex].Color.rgb;
}