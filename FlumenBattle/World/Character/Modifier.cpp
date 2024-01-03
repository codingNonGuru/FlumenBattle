#include "Modifier.h"

using namespace world::character;

#define MODIFIER_COUNT_PER_CHARACTER 16

void ModifierManager::Initialize()
{
    //modifiers.Initialize(MODIFIER_COUNT_PER_CHARACTER);
}

void ModifierManager::AddModifier(Modifier modifier)
{
    *modifiers.Add() = modifier;
}

void ModifierManager::ClearModifiers()
{
    modifiers.Reset();
}

int ModifierManager::GetAmount(Modifiers type)
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