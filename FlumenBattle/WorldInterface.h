#pragma once

#include "FlumenEngine/Core/Singleton.h"

class WorldInfoPanel;
class Element;

namespace world
{
    class WorldDecisionMenu;
    class WorldHoverInfo;
    class GroupEngageMenu;

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

        GroupEngageMenu *engageMenu;

        Array <settlement::SettlementLabel *> settlementLabels;

        void Update();

        void HandlePlayerEncounter();

        void HandlePlayerDisengage();

        void HandlePlayerBattle();

        void HandleBattleEnded();

    public:
        WorldInterface();

        void Initialize();

        void Enable();

        void Disable();    
    };
}