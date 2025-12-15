#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 4) uniform float depth;

// DATA TYPES

struct RiverData
{
    vec4 Color;

	vec2 Positions[3];

    float Thicknesses[3];
};

// DATA BUFFERS

layout (std430, binding = 0) buffer RIVER_DATAS
{
	RiverData riverDatas[];	
};

// TEXTURES

// OUTPUT

out vec3 color;

// DATA STRUCTURES

const uint segmentCount = uint(6);

void main()
{	
    uint objectIndex = uint(gl_VertexID / (6 * int(segmentCount)));

    vec2 firstPosition = riverDatas[objectIndex].Positions[0];
    vec2 secondPosition = riverDatas[objectIndex].Positions[1];
    vec2 thirdPosition = riverDatas[objectIndex].Positions[2];

    uint segmentId = uint(gl_VertexID % (6 * int(segmentCount)));
    segmentId = segmentId / uint(6);

    uint vertexId = uint(gl_VertexID % 6);

    vec2 positions[segmentCount + uint(1)];
    vec2 directions[segmentCount + uint(1)];
    float thicknesses[segmentCount + uint(1)];

    for(int i = 0; i < int(segmentCount) + 1; ++i)
    {
        float t = float(i) / float(segmentCount);

        positions[i] = (1.0f - t) * (1.0f - t) * firstPosition + 2.0f * (1.0f - t) * t * secondPosition + t * t * thirdPosition;

        directions[i] = 2.0f * (1.0f - t) * (secondPosition - firstPosition) + 2.0f * t * (thirdPosition - secondPosition);

        directions[i] = directions[i] / length(directions[i]);
        directions[i] = vec2(-directions[i].y, directions[i].x);

        thicknesses[i] = riverDatas[objectIndex].Thicknesses[0] * (1.0f - t) + riverDatas[objectIndex].Thicknesses[2] * t;
    }

    float thickness = riverDatas[objectIndex].Thicknesses[0];

    vec2 corners[4] = vec2[4]
    (
        positions[segmentId] + directions[segmentId] * thicknesses[segmentId],
        positions[segmentId] - directions[segmentId] * thicknesses[segmentId],
        positions[segmentId + uint(1)] + directions[segmentId + uint(1)] * thicknesses[segmentId + uint(1)],
        positions[segmentId + uint(1)] - directions[segmentId + uint(1)] * thicknesses[segmentId + uint(1)]
    );

    uint indices[6] = uint[6]
    (
        0, 1, 2, 1, 2, 3
    );

    vec2 position = corners[indices[vertexId]];

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);

    color = riverDatas[objectIndex].Color.rgb;
}