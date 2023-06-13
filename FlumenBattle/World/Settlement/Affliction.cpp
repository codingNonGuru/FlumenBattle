#include "Affliction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

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
    auto result = AfflictionResultTypes::NONE;

    auto sewageBonus = settlement.GetModifier(SettlementModifiers::SAVING_THROWS_AGAINST_DISEASE);
    auto diceRoll = utility::GetRandom(1, 20);
    if(diceRoll + sewageBonus >= affliction.Type->Throw)
    {
        if(diceRoll == 20)
        {
            settlement.afflictions.Remove(&affliction);
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
        settlement.population--;
        result = AfflictionResultTypes::DEATH;
        //std::cout<<"death! "<<settlement.GetName()<<"\n";
    }

    affliction.TimeLapsed++;
    if(affliction.TimeLapsed == affliction.Type->Duration)
    {
        settlement.afflictions.Remove(&affliction);
    }

    return {AfflictionTypes::MALARIA, result};
}

AfflictionResult AfflictionPerformer::PerformHunger(Settlement &settlement, Affliction &affliction)
{
    auto result = AfflictionResultTypes::NONE;

    auto foodSecurity = settlement.GetFoodSecurity();

    if(foodSecurity == FoodSecurity::SORELY_LACKING || foodSecurity == FoodSecurity::LACKING)
    {
        auto foodPrivationPenalty = foodSecurity == FoodSecurity::SORELY_LACKING ? 3 : (foodSecurity == FoodSecurity::LACKING ? 1 : 0);
        auto foodAbundanceBonus = foodSecurity == FoodSecurity::CORNUCOPIA ? 2 : (foodSecurity == FoodSecurity::ABUNDANT ? 1 : 0);

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
    else if(foodSecurity == FoodSecurity::CORNUCOPIA || foodSecurity == FoodSecurity::ABUNDANT || foodSecurity == FoodSecurity::ENOUGH)
    {
        auto diceRoll = utility::GetRandom(1, 20);
        if(diceRoll >= affliction.Type->Throw)
        {
            affliction.Stage--;
            if(affliction.Stage == 0)
            {
                settlement.afflictions.Remove(&affliction);
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
            settlement.population--;
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