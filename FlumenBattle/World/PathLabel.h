#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world
{
    class WorldTile;

    class PathLabel : public Element
    {
        Text * label;

        WorldTile *tile;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void SetTile(WorldTile *newTile) {tile = newTile;}
    };
}