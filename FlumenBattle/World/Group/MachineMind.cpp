#include "FlumenBattle/World/Group/MachineMind.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Path.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/Config.h"

namespace world::group
{
    #define ADVENTURER_FIGHT_DC 10

    static auto nearbyPassableTiles = container::Array <WorldTile *> (6);

    void MachineMind::DetermineAction(GroupCore &group) const 
    {
        if(group.isAlive == false)
            return;

        if(group.IsInEncounter() == true)
            return;

        if(group.type->Class == GroupClasses::MERCHANT)
        {
            DetermineActionAsMerchant(group);
        }
        else if(group.type->Class == GroupClasses::ADVENTURER)
        {
            DetermineActionAsAdventurer(group);
        }
        else if(group.type->Class == GroupClasses::BANDIT)
        {
            DetermineActionAsBandit(group);
        }
        else if(group.type->Class == GroupClasses::PATROL)
        {
            DetermineActionAsPatrol(group);
        }
        else if(group.type->Class == GroupClasses::GARRISON)
        {
            DetermineActionAsGarrison(group);
        }
        else if(group.type->Class == GroupClasses::RAIDER)
        {
            DetermineActionAsRaider(group);
        }
    }

    void MachineMind::DetermineActionAsMerchant(GroupCore &group) const 
    {
        if(group.travelActionData->IsOnRoute)
        {
            if(group.GetDestination() == nullptr)
            {
                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
            }
        }
        else
        {
            if(NeedsRest(group) == true)
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

                    group.travelActionData->PlannedDestinationCount = route.GetSize() - 1;
                    for(int i = 1; i < route.GetSize(); ++i)
                    {
                        group.travelActionData->Route[i - 1] = *route[i];
                    }
                    group.travelActionData->IsOnRoute = true;

                    group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
                }
            }
        }
    }

    void MachineMind::DetermineActionAsAdventurer(GroupCore &group) const 
    {
        if(group.travelActionData->IsOnRoute)
        {
            if(group.GetDestination() == nullptr)
            {
                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
            }
        }
        else
        {
            if(group.tile != group.home->GetLocation())
            {
                if(group.home->GetCurrentExploration().Tile != group.GetTile())
                {
                    group.hasAchievedObjective = true;
                }

                if(group.hasAchievedObjective == false)
                {
                    auto bonus = group.home->GetModifier(settlement::Modifiers::PATROL_ATTACK_ROLLS);

                    bonus += group.GetLevel() * 2;

                    auto fightAttempt = utility::RollD20Dice(ADVENTURER_FIGHT_DC, bonus);
                    if(fightAttempt.IsCriticalSuccess() == true)
                    {
                        group.hasAchievedObjective = true;
                        group.money += 200;
                        group.isAlive = true;
                        group.GainExperience(1000);

                        if(group.hasMission == true)
                        {
                            group.home->AddExplorationProgress(20);
                            group.hasMission = false;
                        }
                    }
                    else if(fightAttempt.IsNormalSuccess() == true)
                    {
                        group.hasAchievedObjective = true;
                        group.money += 100;
                        group.isAlive = true;
                        group.GainExperience(1000);

                        if(group.hasMission == true)
                        {
                            group.home->AddExplorationProgress(10);
                            group.hasMission = false;
                        }
                    }
                    else if(fightAttempt.IsRegularFailure() == true)
                    {
                        group.isAlive = false;
                        group.hasAchievedObjective = false;
                        group.CancelAction();
                        return;
                    }
                    else if(fightAttempt.IsCriticalFailure() == true)
                    {
                        group.isAlive = false;
                        group.hasAchievedObjective = false;
                        group.CancelAction();
                        return;
                    }
                }
            }
            else
            {
                group.hasAchievedObjective = false;
            }

            if(NeedsRest(group) == true)
            {
                group.SelectAction(GroupActions::TAKE_LONG_REST);
            }
            else
            {
                auto destination = [&] () -> WorldTile *
                {
                    if(group.tile == group.home->GetLocation())
                    {
                        const auto currentExploration = group.home->GetCurrentlyExploredTile();
                        if(currentExploration != nullptr)
                        {
                            group.hasMission = true;
                            return currentExploration;
                        }

                        auto nearbyTiles = group.tile->GetTileRing(3);
                        while(true)
                        {
                            auto randomTile = *nearbyTiles.Tiles.GetRandom();
                            if(randomTile->HasRelief(world::WorldReliefs::SEA) == false)
                            {
                                group.hasMission = false;
                                return randomTile;
                            }
                        }
                    }
                    else
                    {
                        return group.home->GetLocation();
                    }
                } ();

                auto distance = group.tile->GetDistanceTo(*destination);

                auto pathData = utility::Pathfinder <WorldTile>::Get()->FindPathDjikstra(destination, group.tile, distance + 1);

                group.travelActionData->PlannedDestinationCount = pathData.Tiles.GetSize() - 1;
                for(int i = 1; i < pathData.Tiles.GetSize(); ++i)
                {
                    group.travelActionData->Route[i - 1] = pathData.Tiles.Get(i)->Tile;
                }
                group.travelActionData->IsOnRoute = true;

                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
            }
        }
    }

    void MachineMind::DetermineActionAsBandit(GroupCore &group) const 
    {
        if(group.GetAction() == nullptr)
        {
            if(group.travelActionData->IsOnRoute)
            {
                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
                return;
            }    
        }

        if(group.travelActionData->IsOnRoute)
        {
            if(group.GetDestination() == nullptr)
            {
                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
            }
        }
        else
        {
            if(NeedsRest(group) == true)
            {
                group.SelectAction(GroupActions::TAKE_LONG_REST);
            }
            else
            {
                auto nearbyTiles = group.tile->GetNearbyTiles();
                auto randomTile = *nearbyTiles.GetRandom();

                auto distanceFromHome = group.tile->GetDistanceTo(*group.home->GetLocation());
                auto randomTileDistanceFromHome = randomTile->GetDistanceTo(*group.home->GetLocation());

                bool isTooNear = randomTileDistanceFromHome < 2 && randomTileDistanceFromHome < distanceFromHome;
                bool isTooFar = randomTileDistanceFromHome > 3 && randomTileDistanceFromHome > distanceFromHome;
                if(isTooNear || isTooFar)
                {
                    if(utility::RollD100Dice() > randomTileDistanceFromHome > 4 ? 0 : 30)
                    {
                        randomTile = *nearbyTiles.GetRandom();
                    }
                }

                group.travelActionData->PlannedDestinationCount = 1;
                group.travelActionData->Route[0] = randomTile;
                group.travelActionData->IsOnRoute = true;

                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
            }
        }
    }

    void MachineMind::DetermineActionAsPatrol(GroupCore &group) const
    {
        bool mustResumeAction = group.GetAction() == nullptr;
        if(mustResumeAction == true)
        {
            if(group.travelActionData->IsOnRoute)
            {
                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
                return;
            }    
        }

        bool needsRestBeforeFighting = group.IsDoing(GroupActions::TAKE_LONG_REST) == true || NeedsRest(group) == true;
        if(needsRestBeforeFighting == false)
        {
            auto nearbyGroups = WorldScene::Get()->GetNearbyGroups(group.GetTile(), 0);

            for(auto &nearbyGroup : nearbyGroups.Groups)
            {
                if(nearbyGroup->IsAlive() == false)
                    continue;

                if(nearbyGroup->GetClass() != GroupClasses::BANDIT)
                    continue;

                if(nearbyGroup->IsInEncounter() == true)
                    continue;

                WorldScene::Get()->InitiateEncounter(&group, nearbyGroup);
                return;
            }
        }

        if(group.travelActionData->IsOnRoute)
        {
            if(group.GetDestination() == nullptr)
            {
                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
            }
        }
        else
        {
            if(NeedsRest(group) == true)
            {
                group.SelectAction(GroupActions::TAKE_LONG_REST);
            }
            else
            {
                auto nearbyTiles = group.tile->GetNearbyTiles();
                auto randomTile = *nearbyTiles.GetRandom();

                auto distanceFromHome = group.tile->GetDistanceTo(*group.home->GetLocation());
                auto randomTileDistanceFromHome = randomTile->GetDistanceTo(*group.home->GetLocation());

                bool isTooNear = randomTileDistanceFromHome < 1 && randomTileDistanceFromHome < distanceFromHome;
                bool isTooFar = randomTileDistanceFromHome > 2 && randomTileDistanceFromHome > distanceFromHome;
                if(isTooNear || isTooFar)
                {
                    if(utility::RollD100Dice() > randomTileDistanceFromHome > 3 ? 0 : 20)
                    {
                        randomTile = *nearbyTiles.GetRandom();
                    }
                }

                group.travelActionData->PlannedDestinationCount = 1;
                group.travelActionData->Route[0] = randomTile;
                group.travelActionData->IsOnRoute = true;

                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
            }
        }
    }

    void MachineMind::DetermineActionAsGarrison(GroupCore &group) const
    {
        if(NeedsRest(group) == false)
            return;

        group.SelectAction(GroupActions::TAKE_LONG_REST);
    }

    void MachineMind::DetermineActionAsRaider(GroupCore &group) const
    {
        if(group.travelActionData->IsOnRoute)
        {
            if(group.GetDestination() == nullptr)
            {
                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
            }
        }
        else
        {
            if(NeedsRest(group) == true)
            {
                group.SelectAction(GroupActions::TAKE_LONG_REST);
            }
            else
            {
                auto path = [&] () -> settlement::Path *
                {
                    if(group.tile == group.home->GetLocation())
                    {
                        if(group.home->GetLinks().GetSize() == 0)
                            return nullptr;

                        auto link = group.home->GetLinks().GetRandom();
                        if(link != nullptr /*&& link->Other->GetPolity() != group.home->GetPolity()*/)
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

                    group.travelActionData->PlannedDestinationCount = route.GetSize() - 1;
                    for(int i = 1; i < route.GetSize(); ++i)
                    {
                        group.travelActionData->Route[i - 1] = *route[i];
                    }
                    group.travelActionData->IsOnRoute = true;

                    group.SelectAction(GroupActions::TRAVEL, {group.travelActionData->Route[0]});
                }
            }
        }
    }

    void MachineMind::RegisterActionPerformance(GroupCore &, GroupActionResult) const
    {

    }

    bool MachineMind::NeedsRest(const GroupCore &group) const
    {
        static const auto FATIGUE_ONSET_SINCE_REST = engine::ConfigManager::Get()->GetValue(game::ConfigValues::FATIGUE_ONSET_SINCE_REST).Integer;

        return group.timeSinceLongRest >= FATIGUE_ONSET_SINCE_REST * WorldTime::HOUR_SIZE;
    }
}