#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class SimpleList;

namespace world::settlement
{
    struct RaceGroup;
}

namespace world::interface
{
    class Counter;

    class SetItem : public Element
    {
        Counter *counter;

        int value;

        void HandleConfigure() override;

    public:
        void Setup(Word, int);

        void SetParameters(Position2, float);
    };

    class CounterSet : public Element
    {
        SimpleList *list;

        container::Array <SetItem *> items;

        float distanceFactor;

        Position2 itemCounterPosition;

        float itemCounterSize;

        void HandleConfigure() override;

    public:
        void Setup(const container::Array <settlement::ResourceData> &);

        void Setup(const container::Array <settlement::RaceGroup> &);

        void SetDistanceFactor(float);

        void SetItemParameters(float, Position2, float);
    };
}