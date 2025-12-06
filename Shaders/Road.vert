#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 4) uniform float depth;

// DATA STRUCTURES

struct RoadData
{
    vec4 Color;

	vec2 Position;

    float Rotation;

    float Length;

    float Thickness;
};

// DATA BUFFERS

layout (std430, binding = 0) buffer ROAD_DATAS
{
	RoadData roadDatas[];	
};

// TEXTURES

// OUTPUT

out vec3 color;

void main()
{	
    uint objectIndex = uint(gl_VertexID / 6);

    float rotation = roadDatas[objectIndex].Rotation;

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
    
    position = vec2(position.x * roadDatas[objectIndex].Length, position.y * roadDatas[objectIndex].Thickness);

    float xnew = position.x * c - position.y * s;
    float ynew = position.x * s + position.y * c;

    position = vec2(xnew, ynew) + roadDatas[objectIndex].Position;

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);

    color = roadDatas[objectIndex].Color.rgb;
}