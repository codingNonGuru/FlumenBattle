#include "Modifier.h"

using namespace world::settlement;

void ModifierManager::AddModifier(Modifier modifier)
{
    *modifiers.Add() = modifier;
}

void ModifierManager::ClearModifiers()
{
    modifiers.Reset();
}

ModifierResult ModifierManager::GetAmount(Modifiers type)
{
    auto amount = 0;

    bool hasFound = false;
    for(auto modifier : modifiers)
    {
        if(modifier.Type == type)
        {
            amount += modifier.Amount;
            hasFound = true;
        }
    }

    return {amount, hasFound};
}