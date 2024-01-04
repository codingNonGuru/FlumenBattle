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

    namespace interface
    {
        class InventoryMenu;
    }

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

        interface::InventoryMenu *inventoryMenu;

        bool isInInventoryMode {false};

        Array <settlement::SettlementLabel *> settlementLabels;

        Array <PathLabel *> pathLabels;

        void Update();

        void HandlePlayerEncounter();

        void HandlePlayerDisengage();

        void HandlePlayerBattle();

        void HandleBattleEnded();

        void HandleSettlementFounded();

        void HandleInventoryPressed();

        void HandleCharacterSelected();

    public:
        WorldInterface();

        void Initialize();

        void Enable();

        void Disable();    
    };
}