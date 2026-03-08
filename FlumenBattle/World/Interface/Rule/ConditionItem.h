#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::settlement
{
    class Condition;
}

namespace world::interface::rule
{
    class ConditionItem : public Element
    {
        Text *label;

        void HandleConfigure() override;

    public:
        void Setup(const settlement::Condition *);
    };
}