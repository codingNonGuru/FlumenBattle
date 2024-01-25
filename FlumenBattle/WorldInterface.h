#pragma once

#include "FlumenCore/Singleton.h"

class WorldInfoPanel;
class Element;
class Text;

namespace world
{
    class WorldDecisionMenu;
    class WorldHoverInfo;
    class GroupEngageMenu;
    class PathLabel;

    namespace interface
    {
        class InventoryMenu;
        class SettlementMenu;
        class VendorCursor;
        class ItemHoverInfo;
        class ExplorationMenu;
    }

    namespace settlement
    {
        class SettlementLabel;
        class HoverExtension;
    }
    
    class WorldInterface : public core::Singleton <WorldInterface>
    {
        friend class WorldState;

        friend class core::Singleton <WorldInterface>;

        Element *canvas;

        WorldDecisionMenu *decisionMenu;

        WorldInfoPanel *infoPanel;

        WorldHoverInfo *hoverInfo;

        GroupEngageMenu *engageMenu;

        interface::InventoryMenu *inventoryMenu;

        bool isInInventoryMode {false};

        interface::SettlementMenu *settlementMenu;

        interface::ExplorationMenu *explorationMenu;

        bool isInExplorationMode {false};

        container::Array <settlement::SettlementLabel *> settlementLabels;

        container::Array <PathLabel *> pathLabels;

        settlement::HoverExtension *hoverExtension;

        Text *travelLabel;

        interface::VendorCursor *vendorCursor;

        interface::ItemHoverInfo *itemHoverInfo;

        void Update();

        void HandlePlayerEncounter();

        void HandlePlayerDisengage();

        void HandlePlayerBattle();

        void HandleBattleEnded();

        void HandleSettlementFounded();

        void HandleInventoryPressed();

        void HandleCharacterSelected();

        void HandleSellModeEntered();

        void HandleExplorationToggled();

        WorldInterface();

    public:
        void Initialize();

        void Enable();

        void Disable();    

        settlement::HoverExtension *GetHoverExtension() {return hoverExtension;}

        Element *GetCanvas() const {return canvas;}

        bool IsInInventoryMode() const {return isInInventoryMode;}

        interface::ItemHoverInfo *GetItemHoverInfo() {return itemHoverInfo;}
    };
}