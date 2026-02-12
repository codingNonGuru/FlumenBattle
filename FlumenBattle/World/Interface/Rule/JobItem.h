#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::polity
{
    struct WorkInstruction;
}

namespace world::interface
{
    class Counter;
}

namespace world::interface::rule
{
    class RuleMenu;
    class EconomyTab;

    struct JobItem : public Element
    {
        Element *resourceIcon;

        Element *cohortIcon;

        Counter *priorityLabel;

        polity::WorkInstruction *instruction;

        int priority;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleLeftClick() override;

    public:
        void Setup(polity::WorkInstruction *, bool);
    };
}