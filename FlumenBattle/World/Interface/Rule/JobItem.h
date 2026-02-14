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

        EconomyTab *parentTab;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleLeftClick() override;

        void HandleHover() override;

    public:
        void Setup(polity::WorkInstruction *, EconomyTab *, bool);
    };
}