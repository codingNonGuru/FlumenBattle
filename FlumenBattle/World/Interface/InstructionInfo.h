#pragma once

#include "FlumenEngine/Interface/Element.hpp"

namespace world
{
    namespace tile {struct WorldTile;}

    namespace settlement
    {
        class SettlementTile;
    }
}

namespace world::interface
{
    class Counter;

    class InstructionInfo : public Element
    {
        settlement::SettlementTile *tile;

        Counter *counter;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(settlement::SettlementTile *, int);
    };

    class InstructionInfoSet : public Element
    {
        container::Array <InstructionInfo *> infos;

        void HandleConfigure() override;

        void HandlePlaceModeEntered();
    };
}