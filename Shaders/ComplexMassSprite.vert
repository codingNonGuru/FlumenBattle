#version 320 es

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 textureScale;

layout (location = 2) uniform float spriteSize;

layout (location = 3) uniform int hasRotation;

layout (location = 4) uniform float depth;  

layout (location = 5) uniform int hasOpacity;

layout (location = 6) uniform int hasFlip;

// DATA BUFFERS

layout (std430, binding = 0) buffer POSITIONS
{
	vec2 positions[];	
};

layout (std430, binding = 1) buffer TEXTURE_OFFSETS
{
	vec2 textureOffsets[];	
};

layout (std430, binding = 2) buffer OPACITIES
{
	float opacities[];	
};

layout (std430, binding = 3) buffer FLIP_STATES
{
	int isFlipped[];	
};

layout (std430, binding = 4) buffer ROTATIONS
{
	float rotations[];	
};

// TEXTURES

uniform sampler2D diffuse; 

// OUTPUT

out vec2 textureCoordinates;

out float opacity;

void main()
{	
	vec2 vertices[4] = vec2[4](
		vec2(-0.5f, -0.5f), 
		vec2(0.5f, -0.5f), 
		vec2(0.5f, 0.5f), 
		vec2(-0.5f, 0.5f));

	uint indices[6] = uint[6](0, 1, 2, 0, 2, 3);

	uint flippedIndices[6] = uint[6](1, 0, 3, 1, 3, 2);

	uint vertexIndex = uint(gl_VertexID % 6);
	uint objectIndex = uint(gl_VertexID / 6);
	
	uint index;
	if(hasFlip == 1)
		index = isFlipped[objectIndex] == 1 ? flippedIndices[vertexIndex] : indices[vertexIndex];
	else
		index = indices[vertexIndex];

	textureCoordinates = vertices[index] + vec2(0.5f, 0.5f);

	textureCoordinates *= textureScale;

	textureCoordinates += textureOffsets[objectIndex];

    vec2 position = vertices[indices[vertexIndex]];
    
	if(hasRotation == 1)
	{
		float rotation = rotations[objectIndex];

		float s = sin(rotation);
		float c = cos(rotation);

		float xnew = position.x * c - position.y * s;
		float ynew = position.x * s + position.y * c;

		position = vec2(xnew, ynew);
	}

	ivec2 size = textureSize(diffuse, 0);
	position = position * vec2(float(size.x), float(size.y)) * textureScale * spriteSize + positions[objectIndex];

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);

	if(hasOpacity == 1)
	{
		opacity = opacities[objectIndex];
	}
}