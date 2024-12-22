#pragma once

#include "FlumenEngine/Interface/Element.hpp"

namespace world
{
    class WorldTile;

    namespace settlement
    {
        enum class ResourceTypes;
    }
}

namespace world::interface
{
    class Counter;

    class TileResourceInfo : public Element
    {
        struct ResourceItem : public Element
        {
            Position2 position;

            Counter *counter;

            settlement::ResourceTypes resource;

            void HandleConfigure() override;

            void HandleUpdate() override;

        public:
            void Setup(settlement::ResourceTypes);

            void Setup(int, bool);
        };

        WorldTile *tile {nullptr};

        Position2 tilePosition;

        container::Array <ResourceItem *> items;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(WorldTile *);
    };
}