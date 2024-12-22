#pragma once

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Settlement/Types.h"
#include "FlumenBattle/World/Science/Types.h"

namespace world
{
    class WorldTile;
}

namespace world::settlement
{
    class Settlement;

    struct ExplorationReward
    {
        struct ResourceBatch 
        {
            ResourceTypes Type;

            int Amount;
        };

        union ContentData
        {
            ResourceBatch ResourceBatch;

            science::Technologies Technology;

            ContentData() {}

            ContentData(struct ResourceBatch batch) : ResourceBatch(batch) {}

            ContentData(science::Technologies tech) : Technology(tech) {}
        };

        ExplorationRewards Type;

        ContentData Content;
    };

    class ExplorationHandler : public core::Singleton <ExplorationHandler>
    {
    public:
        ExplorationReward GetReward(WorldTile *);
    };
}