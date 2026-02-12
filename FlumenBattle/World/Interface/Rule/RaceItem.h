#pragma once

#include "FlumenEngine/Interface/Element.hpp"

namespace world::interface
{
    class Counter;
}

namespace world::settlement
{
    struct RaceGroup;
}

namespace world::interface::rule
{
    class RaceItem : public Element
    {
        Element *icon;

        Counter *sizeCounter;

        int groupSize;

        void HandleConfigure() override;

    public:
        void Setup(const settlement::RaceGroup *, bool);
    };
}