#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/CharacterFactory.h"
#include "FlumenBattle/RaceFactory.h"
#include "FlumenBattle/ClassFactory.h"
#include "FlumenBattle/World/Group/GroupAction.h"
#include "FlumenBattle/World/Group/GroupActionFactory.h"
#include "FlumenBattle/World/Group/GroupMind.h"

Array <CharacterClasses> classMakeup; /*= {
    CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, 
    CharacterClasses::RANGER, CharacterClasses::RANGER, CharacterClasses::RANGER, 
    CharacterClasses::CLERIC,
    CharacterClasses::WIZARD, CharacterClasses::WIZARD
    };*/

#define FATIGUE_SAVING_THROW_DC 12

#define TIME_UNTIL_FATIGUE_CHECK 96

namespace world::group
{
    Group::Group() {}

    void Group::Initialize(Word name, const GroupType *type, Integer size, Color color, RaceTypes raceType)
    {
        this->type = type;
        this->controller = type->Controller;

        this->name = name;
        this->color = color;

        actionProgress = 0;

        timeSinceLongRest = 0;

        action = nullptr;
        encounter = nullptr;
        tile = nullptr;

        for(int i = 0; i < size; ++i)
        {
            switch(raceType)
            {
                case RaceTypes::DWARF:
                    classMakeup = {CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::CLERIC};
                    break;
                case RaceTypes::HUMAN:
                    classMakeup = {CharacterClasses::FIGHTER, CharacterClasses::CLERIC, CharacterClasses::RANGER, CharacterClasses::WIZARD};
                    break;
                case RaceTypes::ELF:
                    classMakeup = {CharacterClasses::RANGER, CharacterClasses::RANGER, CharacterClasses::WIZARD};
                    break;
                case RaceTypes::GNOME:
                    classMakeup = {CharacterClasses::RANGER, CharacterClasses::CLERIC, CharacterClasses::WIZARD, CharacterClasses::WIZARD};
                    break;
                case RaceTypes::HALFLING:
                    classMakeup = {CharacterClasses::RANGER, CharacterClasses::CLERIC};
                    break;
                case RaceTypes::GOBLIN:
                    classMakeup = {CharacterClasses::RANGER, CharacterClasses::CLERIC};
                    break;
                case RaceTypes::ORC:
                    classMakeup = {CharacterClasses::FIGHTER};
                    break;
            }

            auto dice = utility::GetRandom(0, classMakeup.GetSize() - 1);
            CharacterClasses type = *classMakeup.Get(dice);

            auto race = RaceFactory::BuildRace(raceType);

            CharacterFactory::Create(race, &ClassFactory::BuildClass(type), *this);
        }
    }

    bool Group::IsAlive()
    {
        for(auto &character : characters)
        {
            if(character.IsAlive())
            {
                return true;
            }
        }

        return false;
    }

    void Group::CheckFatigue()
    {
        timeSinceLongRest += 1;

        if(action != nullptr && action->Type == GroupActions::TAKE_LONG_REST)
        {
            return;
        }

        if(timeSinceLongRest < TIME_UNTIL_FATIGUE_CHECK || (timeSinceLongRest - TIME_UNTIL_FATIGUE_CHECK) % 6 != 0)
        {
            return;
        }

        for(auto &character : characters)
        {
            auto bonus = character.GetFortitudeSaveBonus();
            auto savingThrow = utility::RollD20Dice(FATIGUE_SAVING_THROW_DC, bonus);
            if(savingThrow.IsAnyFailure())
            {
                character.AddFatigue();
            }
        }
    }

    void Group::DetermineAction()
    {
        controller->DetermineAction(*this);
    }

    bool Group::ValidateAction(GroupActions actionType, const GroupActionData &actionData)
    {
        auto possibleAction = GroupActionFactory::Get()->BuildAction(actionType);

        return possibleAction->CanPerform(*this, actionData);
    }   

    GroupActionResult Group::SelectAction(GroupActions actionType, const GroupActionData &actionData)
    {
        if(action && action->Type == actionType)
            return;

        action = GroupActionFactory::Get()->BuildAction(actionType);

        actionProgress = 0;

        travelActionData.Intensity = ActionIntensities::NORMAL;

        return action->Initiate(*this, actionData);
    }

    void Group::PerformAction()
    {
        CheckFatigue();

        if(action)
        {
            actionProgress += GetProgressRate();

            auto result = action->Perform(*this);

            controller->RegisterActionPerformance(*this, result);
        }
    }

    void Group::CancelAction()
    {
        action = nullptr;

        actionProgress = 0;
    }

    void Group::IntensifyAction()
    {
        if(action == nullptr)
            return;
            
        if(action->HasVaryingIntensity == false)
            return;

        switch(travelActionData.Intensity)
        {
            case ActionIntensities::LEISURELY:
                travelActionData.Intensity = ActionIntensities::NORMAL;
                break;
            case ActionIntensities::NORMAL:
                travelActionData.Intensity = ActionIntensities::INTENSE;
                break;
        }
    }

    void Group::SlackenAction()
    {
        if(action == nullptr)
            return;

        if(action->HasVaryingIntensity == false)
            return;
            
        switch(travelActionData.Intensity)
        {
            case ActionIntensities::NORMAL:
                travelActionData.Intensity = ActionIntensities::LEISURELY;
                break;
            case ActionIntensities::INTENSE:
                travelActionData.Intensity = ActionIntensities::NORMAL;
                break;
        }
    }

    void Group::EngageGroup(Encounter *_encounter)
    {
        encounter = _encounter;

        SelectAction(GroupActions::ENGAGE);
    }

    void Group::ExitBattle()
    {
        encounter = nullptr;

        CancelAction();
    }

    void Group::SetTile(WorldTile *tile)
    {
        if(this->tile != nullptr)
        {
            this->tile->group = nullptr;
        }

        this->tile = tile;

        this->tile->group = this;
    }

    float Group::GetActionProgress() const
    {
        if(action->HasVaryingIntensity == false)
        {
            return (float)actionProgress / (float)action->GetDuration(*this);   
        }
        else
        {
            return (float)travelActionData.Progress / (float)action->GetDuration(*this);   
        }
    }

    int Group::GetRemainingActionDuration() const
    {
        if(action->HasVaryingIntensity == false)
        {
            return action->GetDuration(*this) - actionProgress;
        }
        else
        {
            return action->GetDuration(*this) - travelActionData.Progress;
        }
    }

    int Group::GetProgressRate() const
    {
        if(action->HasVaryingIntensity == false)
        {
            return GroupAction::ACTION_PROGRESS_RATE;
        }

        switch(travelActionData.Intensity)
        {
            case ActionIntensities::LEISURELY:
                return GroupAction::ACTION_PROGRESS_RATE / 2;
            case ActionIntensities::NORMAL:
                return GroupAction::ACTION_PROGRESS_RATE;
            case ActionIntensities::INTENSE:
                return GroupAction::ACTION_PROGRESS_RATE + GroupAction::ACTION_PROGRESS_RATE / 2;
        }
    }

    bool Group::IsDoing(GroupActions actionType) const 
    {
        return action != nullptr ? action->Type == actionType : false;
    }
}
