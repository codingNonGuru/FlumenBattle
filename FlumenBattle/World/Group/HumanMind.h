#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Group/GroupMind.h"

class Delegate;

namespace world
{
    class WorldTile;

    namespace character
    {
        struct Item;
    }
}

namespace utility
{
    template <typename TileType>
    struct PathData;
}

namespace world::group
{
    struct GroupActionResult;

    class HumanMind : public GroupMind, public core::Singleton <HumanMind>
    {
        friend class core::Singleton <HumanMind>;

        bool isSellModeActive {false};

        HumanMind();

        void DetermineAction(Group &) const override;

        void RegisterActionPerformance(Group &, GroupActionResult) const override;

        void RegisterActionInitiation(Group &, GroupActionResult) const override;

        void HandleSearch();

        void HandleTakeQuickRest();

        void HandleTakeLongRest();

        void HandleTravel();

        void HandleResumeTravel();

        void HandleCancelTravel();

        void HandleSlackenAction();

        void HandleIntensifyAction();

        void HandleSellModeEntered();

        void HandleSellModeExited();

    public:
        Delegate *OnActionSelected;

        Delegate *OnActionPerformed;

        Delegate *OnActionInitiated;

        Delegate *OnSkillCheckRolled;

        Delegate *OnItemAdded;

        Delegate *OnSellModeEntered;

        void EnableInput();

        void DisableInput();

        void BuyFood();

        void SellItem(character::Item *);

        const GroupActionResult & GetSelectedActionResult();

        const GroupActionResult & GetPerformedActionResult();

        const utility::PathData <WorldTile> GetFullPathData();

        world::WorldTile *GetFinalDestination() const;

        bool IsSellModeActive() const {return isSellModeActive;}
    };  
};