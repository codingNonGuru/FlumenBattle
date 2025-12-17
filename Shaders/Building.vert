#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform float depth;

layout (location = 2) uniform int mode;

struct BuildingData
{
    vec4 Color;

    vec2 Position;

    vec2 Size;

    float Rotation;
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

    vec2 size = buildingData[buildingIndex].Size;

    float s = sin(rotation);
    float c = cos(rotation);

    vec2 position = vertices[vertexId];
    
    position = vec2(position.x * size.x, position.y * size.y);

    float xnew = position.x * c - position.y * s;
    float ynew = position.x * s + position.y * c;

    position = vec2(xnew, ynew) + buildingData[buildingIndex].Position;
    if(mode == 1) 
        position += vec2(3.0f, -1.0f);

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);

    if(mode == 0)
    {
        color = buildingData[buildingIndex].Color.rgb;
    }
    else
    {
        color = vec3(0.0f);
    }
}