#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world
{
    namespace tile {struct WorldTile;}

    class PathLabel : public Element
    {
        Text * label;

        tile::WorldTile *tile;

        int value;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void SetTile(tile::WorldTile *newTile, int newValue) 
        {
            tile = newTile;
            
            value = newValue;
        }
    };
}