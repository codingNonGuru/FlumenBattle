#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 position;

layout (location = 2) uniform float length;

layout (location = 3) uniform float thickness;

layout (location = 4) uniform float rotation;

layout (location = 5) uniform vec4 color;

layout (location = 6) uniform float opacity;

layout (location = 7) uniform float depth;  

// TEXTURES

// OUTPUT

void main()
{	
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

    vec2 vertexPosition = vertices[gl_VertexID % 6];
    
    vertexPosition = vec2(vertexPosition.x * length, vertexPosition.y * thickness);

    float xnew = vertexPosition.x * c - vertexPosition.y * s;
    float ynew = vertexPosition.x * s + vertexPosition.y * c;

    vertexPosition = vec2(xnew, ynew) + position;

	gl_Position = viewMatrix * vec4(vertexPosition.x, vertexPosition.y, depth, 1.0f);
}