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
#include "FlumenBattle/World/Group/GroupController.h"

Array <CharacterClasses> classMakeup; /*= {
    CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, 
    CharacterClasses::RANGER, CharacterClasses::RANGER, CharacterClasses::RANGER, 
    CharacterClasses::CLERIC,
    CharacterClasses::WIZARD, CharacterClasses::WIZARD
    };*/

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

        action = nullptr;
        battle = nullptr;
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

    void Group::DetermineAction()
    {
        controller->DetermineAction(*this);
    }

    bool Group::ValidateAction(world::GroupActions actionType, const GroupActionData &actionData)
    {
        auto possibleAction = GroupActionFactory::Get()->BuildAction(actionType);

        return possibleAction->CanPerform(*this, actionData);
    }   

    void Group::SelectAction(world::GroupActions actionType)
    {
        if(action && actionType == action->Type)
            return;

        action = GroupActionFactory::Get()->BuildAction(actionType);

        actionProgress = 0;
    }

    void Group::PerformAction()
    {
        if(action)
        {
            actionProgress++;

            action->Perform(*this);
        }
    }

    void Group::CancelAction()
    {
        action = nullptr;//GroupActionFactory::Get()->BuildAction(actionType);

        actionProgress = 0;
    }

    void Group::EnterBattle(Battle *_battle)
    {
        battle = _battle;

        SelectAction(world::GroupActions::FIGHT);
    }

    void Group::ExitBattle()
    {
        battle = nullptr;

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
}
