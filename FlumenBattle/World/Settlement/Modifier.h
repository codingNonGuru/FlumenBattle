#pragma once

#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/Types.h"

namespace world
{
    class Settlement;
}

namespace world::settlement
{
    struct Modifier
    {
        SettlementModifiers Type;

        int Amount;

        bool operator==(SettlementModifiers type) {return Type == type;}
    };

    class ModifierSet
    {
    protected:
        container::Array <Modifier> modifiers;

        ModifierSet() {}
    };

    class ModifierManager : protected ModifierSet
    {
        friend class world::Settlement;

        void Initialize();

        void AddModifier(Modifier);

        void ClearModifiers();

        int GetAmount(SettlementModifiers);
    };
}