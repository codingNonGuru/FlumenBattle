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

        int value;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void SetTile(WorldTile *newTile, int newValue) 
        {
            tile = newTile;
            
            value = newValue;
        }
    };
}