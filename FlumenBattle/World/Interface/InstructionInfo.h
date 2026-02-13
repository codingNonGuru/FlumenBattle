#pragma once

#include "FlumenEngine/Interface/Element.hpp"

namespace world
{
    namespace tile {struct WorldTile;}

    namespace settlement
    {
        class SettlementTile;
    }

    namespace polity
    {
        class WorkInstruction;
    }
}

namespace world::interface
{
    namespace rule
    {
        class ResourceHoverInfo;
    }

    class Counter;

    class InstructionInfo : public Element
    {
        settlement::SettlementTile *tile;

        Counter *counter;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(const polity::WorkInstruction *);
    };

    class InstructionInfoSet : public Element
    {
        container::Array <InstructionInfo *> infos;

        rule::ResourceHoverInfo *resourceHoverInfo;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleDisable() override;

        void HandlePlaceModeEntered();
    };
}