#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/Types.hpp"

#define ADD_FRIEND_CLASSES friend class Character;\
    friend class Combatant;\
    friend class SpellCaster;\
    friend class container::Pool<Condition>;\

class Character;
class Combatant;

class Condition
{
    ADD_FRIEND_CLASSES

    ConditionTypes type;

    Integer duration;

    Integer difficultyClass;

    Combatant *combatant;

    Condition();

    Condition(ConditionTypes, Combatant *, Integer = 0);

    void InitializeCold();

    void InitializeEvasion();

    void Apply();

    void ApplyCold();

    void ApplyEvasion();

    ConditionTypes GetType() const {return type;}

    bool IsActive() const {return duration > 0;}
};
