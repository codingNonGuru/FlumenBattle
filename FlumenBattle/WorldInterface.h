#pragma once

#include "FlumenEngine/Core/Singleton.h"

class WorldInfoPanel;
class Element;

namespace world
{
    class WorldDecisionMenu;
    class SettlementLabel;
    class WorldHoverInfo;
    
    class WorldInterface : public Singleton <WorldInterface>
    {
        friend class WorldState;

        Element *canvas;

        WorldDecisionMenu *decisionMenu;

        WorldInfoPanel *infoPanel;

        WorldHoverInfo *hoverInfo;

        Array <SettlementLabel *> settlementLabels;

        void Update();

    public:
        WorldInterface();

        void Initialize();

        void Enable();

        void Disable();    
    };
}