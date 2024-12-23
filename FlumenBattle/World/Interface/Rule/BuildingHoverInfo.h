#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::settlement
{
    class Building;
}

namespace world::interface::rule
{
    class BuildingItem;

    class BuildingHoverInfo : public Element
    {
        Element *border;

        //LayoutGroup *layout;

        Text *nameLabel;

        Text *workerLabel;

        BuildingItem *hoveredItem {nullptr};

        const settlement::Building *building;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(BuildingItem *);
    };
}