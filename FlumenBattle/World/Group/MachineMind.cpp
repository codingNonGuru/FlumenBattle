#include "FlumenBattle/World/Group/MachineMind.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/Utility/Pathfinder.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

namespace world::group
{
    static auto nearbyPassableTiles = container::Array <WorldTile *> (6);

    void MachineMind::DetermineAction(Group &group) const 
    {
        /*if(group.GetAction())  
            return;

        group.SelectAction(GroupActions::SEARCH);*/

        if(group.travelActionData.IsOnRoute)
        {
            if(group.GetDestination() == nullptr)
            {
                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData.Route[0]});
            }
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

            if(path != nullptr)
            {
                auto destination = group.tile == group.home->GetLocation() ? path->GetOther(group.home) : group.home;
                const auto route = path->GetTilesTo(destination);

                group.travelActionData.PlannedDestinationCount = route.GetSize() - 1;
                for(int i = 1; i < route.GetSize(); ++i)
                {
                    group.travelActionData.Route[i - 1] = *route[i];
                }
                group.travelActionData.IsOnRoute = true;

                group.SelectAction(GroupActions::TRAVEL, {group.travelActionData.Route[0]});
            }
            
            //auto pathData = utility::Pathfinder <WorldTile>::Get()->FindPathDjikstra();
        }

        /*if(group.GetDestination() == nullptr)
        {
            nearbyPassableTiles.Reset();

            auto nearbyTiles = group.GetTile()->GetNearbyTiles();
            for(auto &tile : nearbyTiles)
            {
                if(tile->HasRelief(WorldReliefs::SEA) == true)
                    continue;

                *nearbyPassableTiles.Add() = tile;
            }

            auto destination = nearbyPassableTiles.GetRandom();
            if(destination != nullptr)
            {
                group.SelectAction(GroupActions::TRAVEL, {*destination});
            }
        }*/
    }

    void MachineMind::RegisterActionPerformance(Group &, GroupActionResult) const
    {

    }
}