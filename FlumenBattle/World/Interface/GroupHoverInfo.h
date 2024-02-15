#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::group
{
    struct GroupSpotting;
}

namespace world::interface
{
    class GroupHoverInfo : public Element
    {
        Element *border;

        Text *classLabel;

        Text *originLabel;

        void HandleConfigure() override;

    public:
        void Setup(const group::GroupSpotting &);
    };
}