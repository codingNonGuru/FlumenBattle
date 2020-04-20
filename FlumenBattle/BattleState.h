#pragma once

#include "FlumenEngine/Core/State.hpp"

class BattleScene;
class BattleController;

class BattleState : public State
{
    BattleScene * scene;

    BattleController * controller;

    static BattleState * instance;

    BattleState();

    void HandleEnter() override;

    void HandleExit() override;

public:
    static BattleState * Get()
    {
        if(instance == nullptr)
        {
            instance = new BattleState();
        }

        return instance;
    }
};