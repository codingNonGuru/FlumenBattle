#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class Animation;

namespace world::character
{
    enum class Conditions;
}

namespace battle
{
    class Combatant;
}

namespace battle::interface
{
    class ConditionPopup : public Element
    {
        Text *label;

        Element *backdrop;

        Animation *animation;

        Position2 followedPosition;

        const Combatant *combatant;

        void HandleConfigure() override;

        void ConfigureAnimation();

        void HandleEnable() override;

        void HandleDisable() override;

        void HandleUpdate() override;

    public:
        void Setup(const Combatant *, world::character::Conditions);
    };
}