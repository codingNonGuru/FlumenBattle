#pragma once

#include "FlumenEngine/Core/State.hpp"

namespace battle
{
    class BattleScene;
    class BattleController;
    class BattleInterface;

    class BattleState : public State
    {
        BattleScene * scene;

        BattleController * controller;

        BattleInterface *interface;

        BattleState();

        void HandleEnter() override;

        void HandleExit() override;

    public:
        static BattleState * Get()
        {
            static BattleState state;

            return &state;
        }
    };
}