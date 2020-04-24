#pragma once

#include "FlumenBattle/Singleton.h"

class WorldDecisionMenu;

class WorldInterface : public Singleton <WorldInterface>
{
    friend class WorldState;

    WorldDecisionMenu *decisionMenu;

public:
    WorldInterface();

    void Enable();

    void Disable();    
};