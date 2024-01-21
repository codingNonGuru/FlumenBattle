#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Character/Types.h"

namespace world::character
{
    struct Ability 
    {
        friend class AbilityHandler;

    private:
        Ability(AbilityTypes type, Integer score) : Type(type), Score(score) 
        {
            Modifier = Score / 2 - 5;
        }

        void SetScore(Integer score)
        {
            Score = score;
            
            Modifier = Score / 2 - 5;
        }

    public:
        AbilityTypes Type;

        Integer Score;

        Integer Modifier;

        Ability() {}

        Word GetName()
        {
            switch(Type)
            {
                case AbilityTypes::STRENGTH:
                    return "Strength";
                case AbilityTypes::DEXTERITY:
                    return "Dexterity";
                case AbilityTypes::CONSTITUTION:
                    return "Constitution";
                case AbilityTypes::WISDOM:
                    return "Wisdom";
                case AbilityTypes::INTELLIGENCE:
                    return "Intelligence";
                case AbilityTypes::CHARISMA:
                    return "Charisma";
            }
        }
    };
}