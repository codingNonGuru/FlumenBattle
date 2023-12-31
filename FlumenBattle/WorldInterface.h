#pragma once

#include "FlumenCore/Singleton.h"

class WorldInfoPanel;
class Element;

namespace world
{
    class WorldDecisionMenu;
    class WorldHoverInfo;
    class GroupEngageMenu;
    class PathLabel;

    namespace settlement
    {
        class SettlementLabel;
    }
    
    class WorldInterface : public core::Singleton <WorldInterface>
    {
        friend class WorldState;

        Element *canvas;

        WorldDecisionMenu *decisionMenu;

        WorldInfoPanel *infoPanel;

        WorldHoverInfo *hoverInfo;

        GroupEngageMenu *engageMenu;

        Array <settlement::SettlementLabel *> settlementLabels;

        Array <PathLabel *> pathLabels;

        void Update();

        void HandlePlayerEncounter();

        void HandlePlayerDisengage();

        void HandlePlayerBattle();

        void HandleBattleEnded();

        void HandleSettlementFounded();

    public:
        WorldInterface();

        void Initialize();

        void Enable();

        void Disable();    
    };
}