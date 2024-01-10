#include "Affliction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Condition.h"

using namespace world::settlement;

const AfflictionType * AfflictionTypeBuilder::BuildMalaria()
{
    static AfflictionType affliction = {
        AfflictionTypes::MALARIA, 
        14,
        4,
        5,
        &AfflictionPerformer::PerformMalaria};
    return &affliction;
}

const AfflictionType * AfflictionTypeBuilder::BuildHunger()
{
    static AfflictionType affliction = {
        AfflictionTypes::HUNGER, 
        14,
        7,
        0,
        &AfflictionPerformer::PerformHunger};
    return &affliction;
}

const AfflictionType * AfflictionTypeBuilder::BuildAffliction(AfflictionTypes afflictionType)
{
    switch(afflictionType)
    {
        case AfflictionTypes::MALARIA:
            return BuildMalaria();
        case AfflictionTypes::HUNGER:
            return BuildHunger();
        //case AfflictionTypes::TUBERCULOSIS:
            //return BuildTakeLongRest();
    }
}

AfflictionResult AfflictionPerformer::PerformMalaria(Settlement &settlement, Affliction &affliction)
{
    auto removeMalaria = [&]
    {
        settlement.afflictions.RemoveAt(&affliction);

        auto duration = 24 * utility::GetRandom(1, 4);

        settlement.AddCondition({Conditions::IMMUNITY_AGAINST_MALARIA, 1, duration});
    };

    auto result = AfflictionResultTypes::NONE;

    auto rollBonus = settlement.GetModifier(Modifiers::SAVING_THROWS_AGAINST_DISEASE);
    rollBonus += settlement.GetModifier(Modifiers::SAVING_THROWS_AGAINST_MALARIA);
    rollBonus += settlement.GetModifier(Modifiers::ALL_DICE_ROLLS);

    auto diceRoll = utility::GetRandom(1, 20);
    if(diceRoll + rollBonus >= affliction.Type->Throw)
    {
        if(diceRoll == 20)
        {
            removeMalaria();
            return {AfflictionTypes::MALARIA, AfflictionResultTypes::CURE};
        }
        else
        {
            affliction.Stage--;
            if(affliction.Stage < 0)
            {
                affliction.Stage = 0;
            }

            result = AfflictionResultTypes::REGRESSION;
        }
    }
    else
    {
        affliction.Stage++;
        if(diceRoll == 1)
        {
            affliction.Stage++;
        }

        result = AfflictionResultTypes::PROGRESSION;

        if(affliction.Stage > affliction.Type->StageCount)
        {
            affliction.Stage = affliction.Type->StageCount;
        }
    }

    //std::cout<<"malaria! "<<settlement.GetName()<<" "<<affliction.Stage<<"\n";

    if(affliction.Stage == affliction.Type->StageCount)
    {
        settlement.KillPopulation();
        result = AfflictionResultTypes::DEATH;
        //std::cout<<"death! "<<settlement.GetName()<<"\n";
    }

    affliction.TimeLapsed++;
    if(affliction.TimeLapsed == affliction.Type->Duration)
    {
        removeMalaria();
    }

    return {AfflictionTypes::MALARIA, result};
}

AfflictionResult AfflictionPerformer::PerformHunger(Settlement &settlement, Affliction &affliction)
{
    auto result = AfflictionResultTypes::NONE;

    auto foodSecurity = settlement.GetResource(ResourceTypes::FOOD)->Abundance;

    if(foodSecurity == AbundanceLevels::SORELY_LACKING || foodSecurity == AbundanceLevels::LACKING)
    {
        auto foodPrivationPenalty = foodSecurity == AbundanceLevels::SORELY_LACKING ? 3 : (foodSecurity == AbundanceLevels::LACKING ? 1 : 0);
        auto foodAbundanceBonus = foodSecurity == AbundanceLevels::CORNUCOPIA ? 2 : (foodSecurity == AbundanceLevels::ABUNDANT ? 1 : 0);

        auto diceRoll = utility::GetRandom(1, 20);
        if(diceRoll + foodAbundanceBonus - foodPrivationPenalty >= affliction.Type->Throw)
        {
            if(diceRoll == 20)
            {
                affliction.Stage--;
                if(affliction.Stage < 0)
                {
                    affliction.Stage = 0;
                }

                result = AfflictionResultTypes::REGRESSION;
                //settlement.afflictions.Remove(&affliction);
                //return {AfflictionTypes::MALARIA, AfflictionResultTypes::CURE};
            }
            else
            {
            }
        }
        else
        {
            affliction.Stage++;
            if(diceRoll == 1)
            {
                affliction.Stage++;
            }

            result = AfflictionResultTypes::PROGRESSION;

            if(affliction.Stage > affliction.Type->StageCount)
            {
                affliction.Stage = affliction.Type->StageCount;
            }
        }
    }
    else if(foodSecurity == AbundanceLevels::CORNUCOPIA || foodSecurity == AbundanceLevels::ABUNDANT || foodSecurity == AbundanceLevels::ENOUGH)
    {
        auto diceRoll = utility::GetRandom(1, 20);
        if(diceRoll >= affliction.Type->Throw)
        {
            affliction.Stage--;
            if(affliction.Stage == 0)
            {
                settlement.afflictions.RemoveAt(&affliction);
            }
        }
    }

    if(affliction.Stage > 4)
    {
        auto diceRoll = utility::GetRandom(1, 20);
        if(diceRoll >= affliction.Type->Throw)
        {
        }
        else
        {
            settlement.KillPopulation();
        }
    }
}

Affliction AfflictionFactory::Create(AfflictionTypes Type) 
{
    switch(Type)
    {
    case AfflictionTypes::MALARIA:
        return {AfflictionTypeBuilder::Get()->BuildAffliction(AfflictionTypes::MALARIA), 0, 0};
    case AfflictionTypes::HUNGER:
        return {AfflictionTypeBuilder::Get()->BuildAffliction(AfflictionTypes::HUNGER), 1, 0};
    }
}