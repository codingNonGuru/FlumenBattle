#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 hexPosition;

layout (location = 2) uniform vec2 hexSize;

layout (location = 4) uniform float depth;  

// DATA BUFFERS

// TEXTURES

// OUTPUT

out vec2 coords;

out vec2 pos;

void main()
{	
    vec2 vertices[4] = vec2[4] (
        vec2(-0.5f, -0.5f), 
        vec2(0.5f, -0.5f), 
        vec2(0.5f, 0.5f), 
        vec2(-0.5f, 0.5f)
        );

    int indices[6] = int[6] (
        0, 1, 2,
        0, 2, 3
        );

    int vertexIndex = indices[gl_VertexID];

    vec2 position = vertices[vertexIndex];
    
    position = vec2(position.x * hexSize.x, position.y * hexSize.y);

    position += hexPosition;

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);

    coords = vertices[vertexIndex];
    coords.y *= -1.0f;
    coords += vec2(0.5f, 0.5f);
    
    pos = position;
}