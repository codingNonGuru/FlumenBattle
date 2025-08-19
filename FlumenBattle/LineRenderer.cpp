#include "FlumenEngine/Render/DataBuffer.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"

#include "LineRenderer.h"

using namespace render;

#define DEFAULT_BUFFER_COUNT 5

static const Word POSITION_KEY = "Position";

static const Word ROTATION_KEY = "Rotation";

static const Word LENGTHS_KEY = "Lengths";

static const Word THICKNESS_KEY = "Thickness";

static const Word COLOR_KEY = "Color";

LineRenderer::LineRenderer(int capacity)
{
    positions.Initialize(capacity);

    rotations.Initialize(capacity);

    lengths.Initialize(capacity);

    thicknesses.Initialize(capacity);

    colors.Initialize(capacity);

    buffers_.Initialize(DEFAULT_BUFFER_COUNT);

    *buffers_.Add(POSITION_KEY) = new DataBuffer(sizeof(Position2) * capacity);

    *buffers_.Add(ROTATION_KEY) = new DataBuffer(sizeof(Float) * capacity);

    *buffers_.Add(LENGTHS_KEY) = new DataBuffer(sizeof(Float) * capacity);

    *buffers_.Add(THICKNESS_KEY) = new DataBuffer(sizeof(Float) * capacity);

    *buffers_.Add(COLOR_KEY) = new DataBuffer(sizeof(Float4) * capacity);
}

void LineRenderer::TransferData(container::Array <Position2> &positions, container::Array <Float> &rotations, container::Array <Float> &lengths, container::Array <Float> &thicknesses, container::Array <Float4> &colors)
{
    (*buffers_.Get(POSITION_KEY))->UploadData(positions.GetStart(), positions.GetMemorySize());

    (*buffers_.Get(ROTATION_KEY))->UploadData(rotations.GetStart(), rotations.GetMemorySize());

    (*buffers_.Get(LENGTHS_KEY))->UploadData(lengths.GetStart(), lengths.GetMemorySize());

    (*buffers_.Get(THICKNESS_KEY))->UploadData(thicknesses.GetStart(), thicknesses.GetMemorySize());

    (*buffers_.Get(COLOR_KEY))->UploadData(colors.GetStart(), colors.GetMemorySize());
}

void LineRenderer::Render(Camera *camera, float opacity)
{
    static auto lineShader = ShaderManager::GetShader("Line");

    lineShader->Bind();

    lineShader->SetConstant(camera->GetMatrix(), "viewMatrix");

    lineShader->SetConstant(opacity, "opacity");

    lineShader->SetConstant(0.0f, "depth");

    (*buffers_.Get(POSITION_KEY))->Bind(0);

    (*buffers_.Get(ROTATION_KEY))->Bind(1);

    (*buffers_.Get(LENGTHS_KEY))->Bind(2);

    (*buffers_.Get(THICKNESS_KEY))->Bind(3);

    (*buffers_.Get(COLOR_KEY))->Bind(4);

    static const auto INDICES_PER_LINE = 6;
    glDrawArrays(GL_TRIANGLES, 0, INDICES_PER_LINE * positions.GetSize());

    lineShader->Unbind();
}

void LineRenderer::RenderLine(Camera *camera, Position2 position, float length, float thickness, float rotation, Float4 color, float opacity)
{
    static auto lineShader = ShaderManager::GetShader("LineSingular");

    lineShader->Bind();

    lineShader->SetConstant(camera->GetMatrix(), "viewMatrix");

    lineShader->SetConstant(opacity, "opacity");

    lineShader->SetConstant(0.0f, "depth");

    lineShader->SetConstant(position, "position");

    lineShader->SetConstant(length, "length");

    lineShader->SetConstant(thickness, "thickness");

    lineShader->SetConstant(rotation, "rotation");

    lineShader->SetConstant(color, "color");

    static const auto INDICES_PER_LINE = 6;
    glDrawArrays(GL_TRIANGLES, 0, INDICES_PER_LINE);

    lineShader->Unbind();
}

void LineRenderer::RenderDottedLine(Camera *camera, Position2 startPosition, Position2 endPosition, float thickness, Float4 color, float opacity, bool willOmitFirstAndLast, bool hasGaussianFalloff)
{
    auto direction = endPosition - startPosition;

    auto directionUnit = glm::normalize(direction);

    auto length = glm::length(direction);

    static auto lineShader = ShaderManager::GetShader("LineSingular");

    lineShader->Bind();

    lineShader->SetConstant(camera->GetMatrix(), "viewMatrix");

    lineShader->SetConstant(opacity, "opacity");

    lineShader->SetConstant(0.0f, "depth");

    lineShader->SetConstant(thickness, "length");

    lineShader->SetConstant(thickness, "thickness");

    auto rotation = atan2(direction.y, direction.x);

    lineShader->SetConstant(rotation, "rotation");

    lineShader->SetConstant(color, "color");

    int dotCount = glm::length(direction) / thickness;

    dotCount = (dotCount / 2) + 1;

    float paddingFactor = glm::length(direction) - (float)(dotCount - 1) * thickness * 2.0f;
    paddingFactor *= 0.5f;
    auto padding = directionUnit * paddingFactor;

    auto increment = directionUnit * thickness * 2.0f;
    
    for(int i = 0; i < dotCount; i++)
    {
        if(willOmitFirstAndLast == true && (i == 0 || i == dotCount - 1))
            continue;

        auto position = startPosition + increment * float(i) + padding;

        lineShader->SetConstant(position, "position");

        if(hasGaussianFalloff == true)
        {
            auto newOpacity = opacity;

            auto center = startPosition * 0.5f + endPosition * 0.5f;
            auto excentricity = glm::length(center - position);

            newOpacity *= exp(-(excentricity * excentricity) / (0.1f * length * length));

            lineShader->SetConstant(newOpacity, "opacity");
        }

        static const auto INDICES_PER_LINE = 6;
        glDrawArrays(GL_TRIANGLES, 0, INDICES_PER_LINE);
    }

    lineShader->Unbind();
}