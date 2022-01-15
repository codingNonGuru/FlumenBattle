#pragma once

#include "FlumenEngine/Core/Singleton.h"

class WorldInfoPanel;
class Element;

namespace world
{
    class WorldDecisionMenu;
    
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
}