#pragma once

#include "FlumenEngine/Core/Singleton.h"

class WorldDecisionMenu;
class WorldInfoPanel;
class Element;

class WorldInterface : public Singleton <WorldInterface>
{
    friend class WorldState;

    Element *canvas;

    WorldDecisionMenu *decisionMenu;

    WorldInfoPanel *infoPanel;

public:
    WorldInterface();

    void Enable();

    void Disable();    
};