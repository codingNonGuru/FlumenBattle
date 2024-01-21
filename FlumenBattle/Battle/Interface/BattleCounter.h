#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Animation;

namespace world::interface
{
    class Counter;
}

namespace battle
{
    class Combatant;
}

namespace battle::interface
{
    class BattleCounter : public Element
    {
        int value;

        Combatant *combatant;

        world::interface::Counter *counter;

        Animation *animation;

        void HandleConfigure() override;

        void ConfigureAnimation();

        void HandleEnable() override;

        void HandleDisable() override;

    public:
        void Setup(int, Combatant *);
    };
}