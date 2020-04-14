#pragma once

#include "FlumenEngine/Core/State.hpp"

class BattleScene;

class BattleState : public State
{
    BattleScene * scene;

    static BattleState * instance;

    void HandleEnter() override;

    void HandleExit() override;

public:
    BattleState();

    static BattleState * Get()
    {
        if(instance == nullptr)
        {
            instance = new BattleState();
        }

        return instance;
    }
};