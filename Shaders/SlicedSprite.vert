#version 320 es

struct Float3
{
	float x, y, z;
};

// CONSTANT ATTRIBUTES

layout (location = 0) uniform mat4 viewMatrix;

layout (location = 1) uniform vec2 spritePosition;

layout (location = 2) uniform vec2 spriteSize;

layout (location = 4) uniform float depth;  

layout (location = 6) uniform vec2 textureOffset;

layout (location = 7) uniform vec2 textureScale;

// DATA BUFFERS

// TEXTURES

// OUTPUT

out vec2 textureCoordinates;

void main()
{	
	float slice = 0.16666f;

	float pixelsPerCorner = 16.0f;

	vec2 vertices[16] = vec2[16](
		vec2(-0.5f, -0.5f), vec2(-slice, -0.5f), vec2(slice, -0.5f), vec2(0.5f, -0.5f), 
		vec2(-0.5f, -slice), vec2(-slice, -slice), vec2(slice, -slice), vec2(0.5f, -slice), 
		vec2(-0.5f, slice), vec2(-slice, slice), vec2(slice, slice), vec2(0.5f, slice), 
		vec2(-0.5f, 0.5f), vec2(-slice, 0.5f), vec2(slice, 0.5f), vec2(0.5f, 0.5f)
		);

	int indices[54] = int[54](
		0, 1, 4, 1, 4, 5, 
		1, 2, 5, 2, 5, 6,
		2, 3, 6, 3, 6, 7,
		4, 5, 8, 5, 8, 9,
		5, 6, 9, 6, 9, 10,
		6, 7, 10, 7, 10, 11,
		8, 9, 12, 9, 12, 13,
		9, 10, 13, 10, 13, 14,
		10, 11, 14, 11, 14, 15
	);

	int sectionTypes[54] = int[54](
		0, 0, 0, 0, 0, 0,
		1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0,
		2, 2, 2, 2, 2, 2,
		3, 3, 3, 3, 3, 3, 
		4, 4, 4, 4, 4, 4,
		0, 0, 0, 0, 0, 0,
		5, 5, 5, 5, 5, 5,
		0, 0, 0, 0, 0, 0
	);

	int displacePatterns[54] = int[54](
		1, 1, 1, 1, 1, 1,
		0, 0, 0, 0, 0, 0,
		2, 2, 2, 2, 2, 2,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 
		0, 0, 0, 0, 0, 0,
		3, 3, 3, 3, 3, 3,
		0, 0, 0, 0, 0, 0, 
		4, 4, 4, 4, 4, 4
	);

	int index = indices[gl_VertexID];

	int sectionType = sectionTypes[gl_VertexID];
	
	textureCoordinates = vertices[index] + vec2(0.5f, 0.5f);

	vec2 position = vertices[index];

	float cornerCorrection = pixelsPerCorner / (0.5f - slice);

	vec2 displacePower = (spriteSize * vec2(0.5f, 0.5f)) - (vec2(cornerCorrection, cornerCorrection) * 0.5f);

	if(sectionType == 0)
	{
		position = position * vec2(cornerCorrection, cornerCorrection);

		if(displacePatterns[gl_VertexID] == 1)
		{
			position = position - displacePower;
		}
		else if(displacePatterns[gl_VertexID] == 2)
		{
			position.x = position.x + displacePower.x;
			position.y = position.y - displacePower.y;
		}
		else if(displacePatterns[gl_VertexID] == 3)
		{
			position.x = position.x - displacePower.x;
			position.y = position.y + displacePower.y;
		}
		else if(displacePatterns[gl_VertexID] == 4)
		{
			position = position + displacePower;
		}
	}
	else
	{
		displacePower = (spriteSize * vec2(0.5f, 0.5f)) - vec2(pixelsPerCorner, pixelsPerCorner);

		if(sectionType == 1)
		{
			position.x *= displacePower.x * (1.0f / slice);
			
			position.y = (position.y + slice) * cornerCorrection - displacePower.y;
		}
		else if(sectionType == 2)
		{
			position.x = (position.x + slice) * cornerCorrection - displacePower.x;

			position.y *= displacePower.y * (1.0f / slice);
		}
		else if(sectionType == 3)
		{
			position.x *= displacePower.x * (1.0f / slice);
			position.y *= displacePower.y * (1.0f / slice);
		}
		else if(sectionType == 4)
		{
			position.x = (position.x - slice) * cornerCorrection + displacePower.x;

			position.y *= displacePower.y * (1.0f / slice);
		}
		else if(sectionType == 5)
		{
			position.x *= displacePower.x * (1.0f / slice);

			position.y = (position.y - slice) * cornerCorrection + displacePower.y;
		}
	}

	position = position + spritePosition;

	//vec2 position = vertices[gl_VertexID] * spriteSize + spritePosition;

	gl_Position = viewMatrix * vec4(position.x, position.y, depth, 1.0f);
}