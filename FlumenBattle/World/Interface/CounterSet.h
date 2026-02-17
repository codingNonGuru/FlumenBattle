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
    struct ResourceData
    {
        settlement::ResourceTypes Type;

        int Amount;
    };

    class Counter;

    class SetItem : public Element
    {
        Counter *counter;

        int value;

        void HandleConfigure() override;

    public:
        void Setup(Word, int);
    };

    class CounterSet : public Element
    {
        SimpleList *list;

        container::Array <SetItem *> items;

        void HandleConfigure() override;

    public:
        void Setup(const container::Array <ResourceData> &);

        void Setup(const container::Array <settlement::RaceGroup> &);
    };
}