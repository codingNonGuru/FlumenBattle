#include "FlumenBattle/World/Group/MachineMind.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Character/Character.h"

namespace world::group
{
    static auto nearbyPassableTiles = container::Array <WorldTile *> (6);

    void MachineMind::DetermineAction(Group &group) const 
    {
        if(group.isAlive == false)
            return;

        if(group.type->Class == GroupClasses::MERCHANT)
        {
            DetermineActionAsMerchant(group);
        }
        else if(group.type->Class == GroupClasses::ADVENTURER)
        {
            DetermineActionAsAdventurer(group);
        }
    }

    void MachineMind::DetermineActionAsMerchant(Group &group) const 
    {
        if(group.travelActionData.IsOnRoute)
        {
            if(group.GetDestination() == nullptr)
            {
                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData.Route[0]});
            }
        }
        else
        {
            if(group.timeSinceLongRest > 96)
            {
                group.SelectAction(GroupActions::TAKE_LONG_REST);
            }
            else
            {
                auto path = [&] () -> settlement::Path *
                {
                    if(group.tile == group.home->GetLocation())
                    {
                        auto link = group.home->GetLinks().GetRandom();
                        if(link != nullptr)
                        {
                            return link->Path;
                        }
                        else
                        {
                            return nullptr;
                        }
                    }
                    else
                    {
                        return group.tile->GetSettlement()->GetPathTo(group.home);
                    }
                } ();

                if(path != nullptr && path->Tiles.GetSize() < TILES_PER_GROUP_ROUTE)
                {
                    auto destination = group.tile == group.home->GetLocation() ? path->GetOther(group.home) : group.home;
                    auto route = path->GetTilesTo(destination);

                    group.travelActionData.PlannedDestinationCount = route.GetSize() - 1;
                    for(int i = 1; i < route.GetSize(); ++i)
                    {
                        group.travelActionData.Route[i - 1] = *route[i];
                    }
                    group.travelActionData.IsOnRoute = true;

                    group.SelectAction(GroupActions::TRAVEL, {group.travelActionData.Route[0]});
                }
            }
        }
    }

    void MachineMind::DetermineActionAsAdventurer(Group &group) const 
    {
        if(group.travelActionData.IsOnRoute)
        {
            if(group.GetDestination() == nullptr)
            {
                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData.Route[0]});
            }
        }
        else
        {
            if(group.tile != group.home->GetLocation())
            {
                if(group.hasAchievedObjective == false)
                {
                    auto bonus = group.home->GetModifier(settlement::Modifiers::PATROL_ATTACK_ROLLS);

                    auto fightAttempt = utility::RollD20Dice(20, bonus);
                    if(fightAttempt.IsAnySuccess() == true)
                    {
                        group.hasAchievedObjective = true;
                        group.money += 100;
                        group.isAlive = false;
                    }
                    else if(fightAttempt.IsCriticalFailure() == true)
                    {
                        group.isAlive = false;
                        group.CancelAction();
                        return;
                    }
                    else if(fightAttempt.IsAnyFailure() == true)
                    {
                        group.isAlive = false;
                        group.hasAchievedObjective = true;
                    }
                }
            }
            else
            {
                group.hasAchievedObjective = false;
            }

            if(group.timeSinceLongRest > 96)
            {
                group.SelectAction(GroupActions::TAKE_LONG_REST);
            }
            else
            {
                auto destination = [&] () -> WorldTile *
                {
                    if(group.tile == group.home->GetLocation())
                    {
                        auto &nearbyTiles = group.tile->GetTileRing(3);
                        while(true)
                        {
                            auto randomTile = *nearbyTiles.GetRandom();
                            if(randomTile->HasRelief(world::WorldReliefs::SEA) == false)
                                return randomTile;
                        }
                    }
                    else
                    {
                        return group.home->GetLocation();
                    }
                } ();

                auto pathData = utility::Pathfinder <WorldTile>::Get()->FindPathDjikstra(destination, group.tile, 4);

                group.travelActionData.PlannedDestinationCount = pathData.Tiles.GetSize() - 1;
                for(int i = 1; i < pathData.Tiles.GetSize(); ++i)
                {
                    group.travelActionData.Route[i - 1] = pathData.Tiles.Get(i)->Tile;
                }
                group.travelActionData.IsOnRoute = true;

                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData.Route[0]});
            }
            
        }
    }

    void MachineMind::RegisterActionPerformance(Group &, GroupActionResult) const
    {

    }
}