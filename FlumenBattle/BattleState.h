#pragma once

#include "FlumenEngine/Core/State.hpp"

class BattleScene;
class BattleController;

class BattleState : public State
{
    BattleScene * scene;

    BattleController * controller;

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