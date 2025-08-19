#pragma once

#include "FlumenEngine/Render/Model.hpp"

namespace render
{
    class LineRenderer : public Model
    {
        container::Array <Position2> positions;

        container::Array <Float> thicknesses;

        container::Array <Float> lengths;

        container::Array <Float> rotations;

        container::Array <Float4> colors;

    public:
        LineRenderer(int);

        void TransferData(container::Array <Position2> &, container::Array <Float> &, container::Array <Float> &, container::Array <Float> &, container::Array <Float4> &);

        void Render(Camera *, float);

        container::Array <Position2> &GetPositions() {return positions;}

        container::Array <Float> &GetThicknesses() {return thicknesses;}

        container::Array <Float> &GetLengths() {return lengths;}

        container::Array <Float> &GetRotations() {return rotations;}

        container::Array <Float4> &GetColors() {return colors;}

        static void RenderLine(Camera *, Position2, float, float, float, Float4, float);

        static void RenderDottedLine(Camera *, Position2, Position2, float, Float4, float, bool, bool);
    };
}