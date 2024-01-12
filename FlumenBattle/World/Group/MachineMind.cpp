#include "FlumenBattle/World/Group/MachineMind.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/WorldTile.h"

namespace world::group
{
    static auto nearbyPassableTiles = container::Array <WorldTile *> (6);

    void MachineMind::DetermineAction(Group &group) const 
    {
        /*if(group.GetAction())  
            return;

        group.SelectAction(GroupActions::SEARCH);*/

        if(group.GetDestination() == nullptr)
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
        }
    }

    void MachineMind::RegisterActionPerformance(Group &, GroupActionResult) const
    {

    }
}