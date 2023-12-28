#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 hexPosition;

layout (location = 2) uniform vec2 hexSize;

layout (location = 4) uniform float depth;  

layout (location = 6) uniform float rotation;

// DATA BUFFERS

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

    vec2 position = vertices[gl_VertexID];
    
    position = vec2(position.x * hexSize.x, position.y * hexSize.y);

    float xnew = position.x * c - position.y * s;
    float ynew = position.x * s + position.y * c;

    position = vec2(xnew, ynew) + hexPosition;

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);
}