#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Core/SceneManager.hpp"

#include "FlumenBattle/CharacterFactory.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleMap.h"

#define ABILITY_COUNT 6

Integer abilityScores[ABILITY_COUNT];

Character* CharacterFactory::Create(CharacterClasses type, Group& group)
{
    auto character = group.characters.Allocate();

    character->group = &group;

    character->type = type;
    character->level = 1;

    character->proficiencyBonus = 2;

    for(auto& score : abilityScores)
    {
        score = GetRandomAbilityScore();
    }

    for(Index i = 0; i <= ABILITY_COUNT / 2; ++i)
    {
        for(Index j = 0; j < ABILITY_COUNT - 1; ++j)
        {
            if(abilityScores[j] < abilityScores[j + 1])
            {
                auto score = abilityScores[j];
                abilityScores[j] = abilityScores[j + 1];
                abilityScores[j + 1] = score;
            }
        }   
    }

    switch(type)
    {
    case CharacterClasses::FIGHTER:
        character->strength = abilityScores[0];
        character->constitution = abilityScores[1];
        character->dexterity = abilityScores[2];
        character->wisdom = abilityScores[3];
        character->charisma = abilityScores[4];
        character->intelligence = abilityScores[5];

        character->hitDiceCount = 10;
        character->armorClass = 16;
        character->primaryAbility = &character->strength;

        character->defaultRange = 1;
        break;
    case CharacterClasses::RANGER:
        character->dexterity = abilityScores[0];
        character->intelligence = abilityScores[1];
        character->constitution = abilityScores[2];
        character->wisdom = abilityScores[3];
        character->charisma = abilityScores[4];
        character->strength = abilityScores[5];

        character->hitDiceCount = 8;
        character->armorClass = 15;
        character->primaryAbility = &character->dexterity;

        character->defaultRange = 10;
        break;
    case CharacterClasses::CLERIC:
        character->wisdom = abilityScores[0];
        character->strength = abilityScores[1];
        character->constitution = abilityScores[2];
        character->charisma = abilityScores[3];
        character->intelligence = abilityScores[4];
        character->dexterity = abilityScores[5];

        character->hitDiceCount = 8;
        character->armorClass = 13;
        character->primaryAbility = &character->wisdom;

        character->defaultRange = 1;
        break;
    case CharacterClasses::WIZARD:
        character->intelligence = abilityScores[0];
        character->constitution = abilityScores[1];
        character->dexterity = abilityScores[2];
        character->wisdom = abilityScores[3];
        character->charisma = abilityScores[4];
        character->strength = abilityScores[5];

        character->hitDiceCount = 6;
        character->armorClass = 11;
        character->primaryAbility = &character->intelligence;
        character->defaultRange = 10;
        break;
    }

    character->maximumHitPoints = character->hitDiceCount + character->constitution.Modifier * character->level;

    auto battleScene = (BattleScene*)SceneManager::Get(Scenes::BATTLE);
    character->tile = battleScene->GetBattleMap()->GetEmptyRandomTile();
    character->tile->Character = character;

    character->defaultSpeed = utility::GetRandom(4, 6);

    character->Initialize();

    return character;
}

Integer CharacterFactory::GetRandomAbilityScore()
{
    Integer dices[4];

    Integer leastDice = 6;

    Index diceIndex = 0;
    Index leastDiceIndex = 0;
    for(auto& dice : dices)
    {
        dice = utility::GetRandom(1, 6);
        if(dice <= leastDice)
        {
            leastDice = dice;
            leastDiceIndex = diceIndex;
        }

        diceIndex++;
    }

    Integer sum = 0;
    for(Index i = 0; i < 4; ++i)
    {
        if(i == leastDiceIndex)
            continue;

        sum += dices[i];
    }

    return sum;
}