#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Core/SceneManager.hpp"

#include "FlumenBattle/CharacterFactory.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleMap.h"

Integer abilityScores[6];

Character* CharacterFactory::Create(CharacterTypes type, Group& group)
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

    for(Index i = 0; i < 6; ++i)
    {
        for(Index j = 0; j < 5; ++j)
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
    case CharacterTypes::FIGHTER:
        character->strength = abilityScores[0];
        character->constitution = abilityScores[1];
        character->dexterity = abilityScores[2];
        character->wisdom = abilityScores[3];
        character->charisma = abilityScores[4];
        character->intelligence = abilityScores[5];

        character->hitDiceCount = 10;
        character->armorClass = 16;
        character->primaryAbility = &character->strength;
        break;
    case CharacterTypes::RANGER:
        character->dexterity = abilityScores[0];
        character->intelligence = abilityScores[1];
        character->constitution = abilityScores[2];
        character->wisdom = abilityScores[3];
        character->charisma = abilityScores[4];
        character->strength = abilityScores[5];

        character->hitDiceCount = 8;
        character->armorClass = 15;
        character->primaryAbility = &character->dexterity;
        break;
    case CharacterTypes::CLERIC:
        character->wisdom = abilityScores[0];
        character->strength = abilityScores[1];
        character->constitution = abilityScores[2];
        character->charisma = abilityScores[3];
        character->intelligence = abilityScores[4];
        character->dexterity = abilityScores[5];

        character->hitDiceCount = 8;
        character->armorClass = 13;
        character->primaryAbility = &character->wisdom;
        break;
    case CharacterTypes::WIZARD:
        character->intelligence = abilityScores[0];
        character->constitution = abilityScores[1];
        character->dexterity = abilityScores[2];
        character->wisdom = abilityScores[3];
        character->charisma = abilityScores[4];
        character->strength = abilityScores[5];

        character->hitDiceCount = 6;
        character->armorClass = 11;
        character->primaryAbility = &character->intelligence;
        break;
    }

    character->hitPointCount = character->hitDiceCount + character->constitution.Modifier * character->level;

    auto battleScene = (BattleScene*)SceneManager::Get(Scenes::BATTLE);
    character->tile = battleScene->GetBattleMap()->GetRandomTile();

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