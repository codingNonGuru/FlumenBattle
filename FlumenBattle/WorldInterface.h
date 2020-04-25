#pragma once

#include "FlumenBattle/Singleton.h"

class WorldDecisionMenu;
class WorldInfoPanel;

class WorldInterface : public Singleton <WorldInterface>
{
    friend class WorldState;

    WorldDecisionMenu *decisionMenu;

    WorldInfoPanel *infoPanel;

public:
    WorldInterface();

    void Enable();

    void Disable();    
};