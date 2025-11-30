#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world
{
    namespace tile {struct WorldTile; struct WorldEdge;}

    class PathLabel : public Element
    {
        Text * label;

        tile::WorldTile *tile;

        tile::WorldTile *otherTile;

        tile::WorldEdge *edge;

        int value;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void SetTile(tile::WorldTile *newTile, int newValue) 
        {
            tile = newTile;
            
            value = newValue;

            edge = nullptr;
        }

        void SetTile(tile::WorldTile *newTile, tile::WorldTile *newOtherTile, int newValue) 
        {
            tile = newTile;
            
            value = newValue;

            otherTile = newOtherTile;
        }

        void SetEdge(tile::WorldEdge *newEdge, int newValue) 
        {
            edge = newEdge;
            
            value = newValue;

            tile = nullptr;
        }
    };
}