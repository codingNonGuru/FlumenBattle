#include "Modifier.h"

using namespace world::settlement;

#define MODIFIER_COUNT_PER_SETTLEMENT 32

void ModifierManager::Initialize()
{
    modifiers.Initialize(MODIFIER_COUNT_PER_SETTLEMENT);
}

void ModifierManager::AddModifier(Modifier modifier)
{
    *modifiers.Add() = modifier;
}

void ModifierManager::ClearModifiers()
{
    modifiers.Reset();
}

int ModifierManager::GetAmount(SettlementModifiers type)
{
    auto amount = 0;

    for(auto modifier : modifiers)
    {
        if(modifier.Type == type)
        {
            amount += modifier.Amount;
        }
    }

    return amount;
}