#pragma once

namespace world
{
    class WorldTile;
}

namespace world::settlement
{
    class Settlement;
}

namespace world::polity
{
    class Polity;

    class Mind
    {
    public:
        virtual void MakeDecision(Polity &) const = 0;

        virtual void UpdateWorkforce(Polity &) const = 0;

        virtual void DecideResearch(Polity &) const = 0;

        virtual void ProcessTrade(Polity &) const = 0;

        virtual void DecideBorders(Polity &) const = 0;

        virtual void RegisterPopIncrease(settlement::Settlement *) const = 0;

        virtual void RegisterTileExplored(settlement::Settlement *, WorldTile *) const = 0;

        virtual void RegisterMarkForDeletion() const = 0;
    };
}