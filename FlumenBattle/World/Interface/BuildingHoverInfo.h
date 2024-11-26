#pragma once

#include "FlumenEngine/Interface/Element.hpp"

namespace world::settlement
{
    class Building;
}

namespace world::interface
{
    class BuildingItem;

    class BuildingHoverInfo : public Element
    {
        Element *border;

        //LayoutGroup *layout;

        Text *nameLabel;

        BuildingItem *hoveredItem {nullptr};

        const settlement::Building *building;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(BuildingItem *);
    };
}