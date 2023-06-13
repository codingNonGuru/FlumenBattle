#pragma once

#include "FlumenEngine/Core/Singleton.h"

class WorldInfoPanel;
class Element;

namespace world
{
    class WorldDecisionMenu;
    class WorldHoverInfo;

    namespace settlement
    {
        class SettlementLabel;
    }
    
    class WorldInterface : public Singleton <WorldInterface>
    {
        friend class WorldState;

        Element *canvas;

        WorldDecisionMenu *decisionMenu;

        WorldInfoPanel *infoPanel;

        WorldHoverInfo *hoverInfo;

        Array <settlement::SettlementLabel *> settlementLabels;

        void Update();

    public:
        WorldInterface();

        void Initialize();

        void Enable();

        void Disable();    
    };
}