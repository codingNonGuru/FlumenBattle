#include "Modifier.h"

using namespace world::character;

void ModifierManager::Initialize() {}

void ModifierManager::AddModifier(Modifier modifier)
{
    *modifiers.Add() = modifier;
}

void ModifierManager::ClearModifiers()
{
    modifiers.Reset();
}

int ModifierManager::GetAmount(Modifiers type) const
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