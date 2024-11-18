#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/Queue.h"

#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/Group/ItemChange.h"

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
        class ReputationMenu;
        class GroupHoverInfo;
        class QuestPopup;
        class QuestMenu;
        class ActionPopup;
        class RollPopup;
        class MoneyPopup;
        class ItemPopup;
        class ProductionDecisionMenu;
        class ConquestPopup;
        class RecruitmentMenu;
        class WorkerPlaceCursor;
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

        container::Array <settlement::SettlementLabel *> settlementLabels;

        container::Array <PathLabel *> pathLabels;

        settlement::HoverExtension *hoverExtension;

        Element *travelBackdrop;

        Text *travelLabel;

        interface::VendorCursor *vendorCursor;

        interface::WorkerPlaceCursor *workerPlaceCursor;

        interface::ItemHoverInfo *itemHoverInfo;

        interface::ReputationMenu *reputationMenu;

        interface::GroupHoverInfo *groupHoverInfo;

        interface::QuestPopup *questPopup;

        interface::QuestMenu *questMenu;

        container::Pool <interface::ActionPopup *> actionPopups;

        container::Pool <interface::RollPopup *> rollPopups;

        container::Pool <interface::MoneyPopup *> moneyPopups;

        container::Pool <interface::ItemPopup *> itemPopups;

        enum class PopupTypes {ACTION, ROLL, MONEY, ITEM};

        struct PopupData
        {
            PopupTypes Type;

            union VariableData
            {
                struct ActionData {group::GroupActions Type; bool HasStarted;} ActionData; 

                utility::Success RollData;

                int MoneyData;

                group::ItemChange ItemChange;

                VariableData(utility::Success rollData) : RollData(rollData) {}

                VariableData(group::GroupActions type, bool hasStarted) : ActionData{type, hasStarted} {}

                VariableData(int moneyData) : MoneyData(moneyData) {}

                VariableData(group::ItemChange itemChange) : ItemChange(itemChange) {}
            } Data;
        };

        container::Queue <PopupData> popupQueue;

        interface::ProductionDecisionMenu *productionMenu;

        interface::ConquestPopup *conquestPopup;

        interface::RecruitmentMenu *recruitmentMenu;

        void Update();

        void HandlePlayerEncounter();

        void HandlePlayerDisengage();

        void HandlePlayerBattle();

        void HandleBattleEnded();

        void HandleSettlementFounded();

        void HandleInventoryPressed();

        void HandleCharacterSelected();

        void HandleSellModeEntered();

        void HandleWorkerPlaceModeEntered();

        void HandleConsoleToggled();

        void HandleMenuCycled();

        void HandleSpottingHovered();

        void HandleQuestStarted();

        void HandleQuestFinished();

        void HandleActionInitiated();

        void HandleDiceRolled();

        void HandlePlayerWealthChanged();

        void HandlePlayerItemChanged();

        void HandleSettlementEntered();

        void HandleSettlementExited();

        void HandlePlayerConquest();

        WorldInterface();

    public:
        void Initialize();

        void Enable();

        void Disable();    

        settlement::HoverExtension *GetHoverExtension() {return hoverExtension;}

        Element *GetCanvas() const {return canvas;}

        bool IsInInventoryMode() const {return isInInventoryMode;}

        interface::ItemHoverInfo *GetItemHoverInfo() {return itemHoverInfo;}

        void RemoveActionPopup(interface::ActionPopup *);

        void RemoveRollPopup(interface::RollPopup *);

        void RemoveMoneyPopup(interface::MoneyPopup *);

        void RemoveItemPopup(interface::ItemPopup *);

        void OpenRecruitmentMenu();
    };
}