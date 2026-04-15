#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform float depth;

layout (location = 2) uniform int maximumGroupSize;

struct GroupData
{
    int ClassIndices[16];

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

vec2 distortedVertices[6] = vec2[6] (
    vec2(-0.5f, -0.5f), 
    vec2(0.5f, -0.5f), 
    vec2(0.2f, 0.5f), //
    vec2(-0.5f, -0.5f), 
    vec2(0.2f, 0.5f), //
    vec2(-0.8f, 0.5f) //
    );

out vec2 texCoords;

void main()
{
    uint groupIndex = uint(gl_VertexID / (6 * maximumGroupSize));

    uint memberIndex = uint((gl_VertexID / 6) % maximumGroupSize);

    uint vertexIndex = uint(gl_VertexID % 6);

    float size = 8.0f;
    if(int(memberIndex) >= groupData[groupIndex].MemberCount)
    {
        size = 0.0f;
    }

    float memberFactor = float(memberIndex);
    float spiralAngle = memberFactor * 2.5f;
    float rangeFactor = 1.5f + spiralAngle * 0.4f;
    vec2 offset = vec2(cos(spiralAngle), sin(spiralAngle)) * rangeFactor;

    vec2 basePosition = groupData[groupIndex].Position + offset;
    vec2 position = basePosition + distortedVertices[vertexIndex] * size + vec2(3.0f, 0.3f);

    texCoords = vertices[vertexIndex] + vec2(0.5f, 0.5f);
    texCoords.x *= 0.1666f;

    int classIndex = groupData[groupIndex].ClassIndices[memberIndex];
    if(classIndex == 0)
    {
        texCoords.x += 0.0f;
    }
    else if(classIndex == 1)
    {
        texCoords.x += 0.5f;
    }
    else if(classIndex == 2)
    {
        texCoords.x += 0.6666f;
    }
    else if(classIndex == 3)
    {
        texCoords.x += 0.8333f;
    }

    gl_Position = viewMatrix * vec4(position.x, position.y, depth + basePosition.y * 0.000001f, 1.0f);
}