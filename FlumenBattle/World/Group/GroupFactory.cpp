#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/World/Group/GroupFactory.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Group/GroupExtraData.h"
#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/Group/CharacterEssence.h"
#include "FlumenBattle/World/Group/GroupAllocator.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Character/CharacterFactory.h"
#include "FlumenBattle/RaceFactory.h"
#include "FlumenBattle/World/Character/ClassFactory.h"
#include "FlumenBattle/PreGame/Types.h"

Array <Color> colors = {Color::RED, Color::GREEN, Color::CYAN, Color::RED * 0.5f, Color::GREEN * 0.5f, Color::GREEN, Color::BLUE};

Array <world::character::CharacterClasses> classMakeup; /*= {
    CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, CharacterClasses::FIGHTER, 
    CharacterClasses::RANGER, CharacterClasses::RANGER, CharacterClasses::RANGER, 
    CharacterClasses::CLERIC,
    CharacterClasses::WIZARD, CharacterClasses::WIZARD
    };*/

namespace world::group
{
    Color GetColor()
    {
        static Integer colorIndex = 0;
        auto color = colors[colorIndex];

        colorIndex++;
        if(colorIndex == colors.GetSize())
            colorIndex = 0;

        return color;
    }

    GroupCore* GroupFactory::CreatePlayerGroup(GroupBuildData buildData)
    {
        auto group = GroupAllocator::Get()->Allocate(true);

        auto type = GroupTypeFactory::BuildGroupType(buildData.Type);

        auto size = buildData.MemberDatas->GetSize();

        auto color = GetColor();

        group->SetMuleCount(0);

        for(auto &memberData : *buildData.MemberDatas)
        {
            auto characterClass = &character::ClassFactory::BuildClass(memberData.Class);

            auto race = RaceFactory::BuildRace(memberData.Race);

            character::CharacterFactory::Create(race, characterClass, *group);
        }

        group->Initialize(type, size, buildData.Race);

        group->SetHome(buildData.Home);

        group->SetTile(buildData.Home->GetLocation());

        return group;
    }

    GroupCore* GroupFactory::CreateMachineGroup(GroupBuildData buildData)
    {
        auto group = GroupAllocator::Get()->Allocate(false);

        auto type = GroupTypeFactory::BuildGroupType(buildData.Type);

        auto size = type->Size; //utility::GetRandom(4, 7);

        auto color = GetColor();

        if(buildData.Type == GroupClasses::MERCHANT)
        {
            group->SetMuleCount(2);
        }
        else
        {
            group->SetMuleCount(0);
        }

        group->characterHandler.characterCount = 0;

        for(auto &essence : *group->characterHandler.characters)
        {
            essence.isFunctioning = false;
        }

        for(int i = 0; i < size; ++i)
        {
            switch(buildData.Race)
            {
                case RaceTypes::DWARF:
                    classMakeup = {character::CharacterClasses::FIGHTER, character::CharacterClasses::FIGHTER, character::CharacterClasses::CLERIC};
                    break;
                case RaceTypes::HUMAN:
                    classMakeup = {character::CharacterClasses::FIGHTER, character::CharacterClasses::CLERIC, character::CharacterClasses::RANGER, character::CharacterClasses::WIZARD};
                    break;
                case RaceTypes::ELF:
                    classMakeup = {character::CharacterClasses::RANGER, character::CharacterClasses::RANGER, character::CharacterClasses::WIZARD};
                    break;
                case RaceTypes::GNOME:
                    classMakeup = {character::CharacterClasses::RANGER, character::CharacterClasses::CLERIC, character::CharacterClasses::WIZARD, character::CharacterClasses::WIZARD};
                    break;
                case RaceTypes::HALFLING:
                    classMakeup = {character::CharacterClasses::RANGER, character::CharacterClasses::CLERIC};
                    break;
                case RaceTypes::GOBLIN:
                    classMakeup = {character::CharacterClasses::RANGER, character::CharacterClasses::CLERIC};
                    break;
                case RaceTypes::ORC:
                    classMakeup = {character::CharacterClasses::FIGHTER};
                    break;
            }

            auto dice = utility::GetRandom(0, classMakeup.GetSize() - 1);
            auto characterClass = *classMakeup.Get(dice);

            auto race = RaceFactory::BuildRace(buildData.Race);

            auto characterEssence = group->characterHandler.AddCharacter();
            
            characterEssence->Initialize(characterClass);
        }

        group->Initialize(type, size, buildData.Race);

        group->SetHome(buildData.Home);

        if(buildData.Type == GroupClasses::BANDIT)
        {
            auto nearbyTiles = buildData.Home->GetLocation()->GetNearbyTiles(2);

            while(true)
            {
                auto randomTile = *nearbyTiles.Tiles.GetRandom();
                if(randomTile != buildData.Home->GetLocation())
                {
                    group->SetTile(randomTile);
                    break;
                }
            }
        }
        else if(buildData.Type == GroupClasses::PATROL)
        {
            auto nearbyTiles = buildData.Home->GetLocation()->GetNearbyTiles(2);

            auto randomTile = *nearbyTiles.Tiles.GetRandom();
            
            group->SetTile(randomTile);
        }
        else
        {
            group->SetTile(buildData.Home->GetLocation());
        }

        return group;
    }

    void GroupFactory::TransformIntoDeepGroup(GroupCore *group)
    {
        if(group->extraData != nullptr)
            return;

        GroupAllocator::Get()->GenerateExtraGroupData(group);

        for(auto &characterEssence : *group->characterHandler.characters)
        {
            auto race = RaceFactory::BuildRace(group->raceType);

            auto character = character::CharacterFactory::Create(race, characterEssence, *group);
        }

        group->extraData->Initialize(group);
    }

    void GroupFactory::RevertToShallowGroup(GroupCore *group)
    {
    }
}