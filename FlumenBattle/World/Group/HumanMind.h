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
    struct GroupSpotting;

    class HumanMind : public GroupMind, public core::Singleton <HumanMind>
    {
        friend class core::Singleton <HumanMind>;

        bool isSellModeActive {false};

        HumanMind();

        void DetermineAction(Group &) const override;

        void RegisterActionPerformance(Group &, GroupActionResult) const override;

        void RegisterActionInitiation(Group &, GroupActionResult) const override;

        void UpdateSpottings();

        void HandleSceneUpdate();

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

        Delegate *OnSettlementEntered;

        Delegate *OnSettlementExited;

        Delegate *OnGroupSpotted;

        Delegate *OnGroupFaded;

        void EnableInput();

        void DisableInput();

        void BuyFood();

        void SellItem(character::Item *);

        void PursueSighting(const GroupSpotting &);

        const GroupActionResult & GetSelectedActionResult();

        const GroupActionResult & GetPerformedActionResult();

        const utility::PathData <WorldTile> GetFullPathData();

        world::WorldTile *GetFinalDestination() const;

        bool IsSellModeActive() const {return isSellModeActive;}

        const GroupSpotting &GetLatestGroupSpotting() const;

        const container::Array <GroupSpotting *> &GetLatestFadings() const;

        const container::Pool <GroupSpotting> &GetGroupSightings() const;
    };  
};