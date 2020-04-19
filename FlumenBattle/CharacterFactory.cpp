#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/CharacterFactory.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleTile.h"
#include "FlumenBattle/Weapon.h"
#include "FlumenBattle/Spell.h"
#include "FlumenBattle/ProficiencyFactory.h"
#include "FlumenBattle/RaceFactory.h"

Integer abilityScores[ABILITY_COUNT];

Character* CharacterFactory::Create(const Race *race, CharacterClasses type, Group& group)
{
    auto character = group.characters.Allocate();

    character->group = &group;

    character->type = type;
    character->level = 1;

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
        character->abilities.SetScore(AbilityTypes::STRENGTH, abilityScores[0]);
        character->abilities.SetScore(AbilityTypes::CONSTITUTION, abilityScores[1]);
        character->abilities.SetScore(AbilityTypes::DEXTERITY, abilityScores[2]);
        character->abilities.SetScore(AbilityTypes::WISDOM, abilityScores[3]);
        character->abilities.SetScore(AbilityTypes::CHARISMA, abilityScores[4]);
        character->abilities.SetScore(AbilityTypes::INTELLIGENCE, abilityScores[5]);

        character->hitDiceCount = 10;
        character->armorClass = 16;

        character->abilities.SetAttackAbility(AbilityTypes::STRENGTH);

        character->actions.Initialize(3);
        *character->actions.Allocate() = {CharacterActions::ATTACK};
        *character->actions.Allocate() = {CharacterActions::DODGE};
        *character->actions.Allocate() = {CharacterActions::DASH};

        character->AddWeapon(WeaponFactory::BuildGreatSword());
        character->AddWeapon(WeaponFactory::BuildHatchet());

        character->AddProficiency(ProficiencyFactory::BuildPerceptionProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildReflexSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildFortitudeSaveProficiency(ProficiencyLevels::TRAINED));
        character->AddProficiency(ProficiencyFactory::BuildWillSaveProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildSimpleWeaponsProficiency(ProficiencyLevels::TRAINED));
        character->AddProficiency(ProficiencyFactory::BuildMartialWeaponsProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildUnarmedCombatProficiency(ProficiencyLevels::TRAINED));
        break;
    case CharacterClasses::RANGER:
        character->abilities.SetScore(AbilityTypes::DEXTERITY, abilityScores[0]);
        character->abilities.SetScore(AbilityTypes::WISDOM, abilityScores[1]);
        character->abilities.SetScore(AbilityTypes::CONSTITUTION, abilityScores[2]);
        character->abilities.SetScore(AbilityTypes::INTELLIGENCE, abilityScores[3]);
        character->abilities.SetScore(AbilityTypes::CHARISMA, abilityScores[4]);
        character->abilities.SetScore(AbilityTypes::STRENGTH, abilityScores[5]);

        character->hitDiceCount = 8;
        character->armorClass = 15;

        character->abilities.SetAttackAbility(AbilityTypes::DEXTERITY);

        character->actions.Initialize(3);
        *character->actions.Allocate() = {CharacterActions::ATTACK};
        *character->actions.Allocate() = {CharacterActions::DODGE};
        *character->actions.Allocate() = {CharacterActions::DASH};

        character->AddWeapon(WeaponFactory::BuildLongBow());
        character->AddWeapon(WeaponFactory::BuildShortSword());

        character->AddProficiency(ProficiencyFactory::BuildPerceptionProficiency(ProficiencyLevels::TRAINED));

        character->AddProficiency(ProficiencyFactory::BuildReflexSaveProficiency(ProficiencyLevels::TRAINED));
        character->AddProficiency(ProficiencyFactory::BuildFortitudeSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildWillSaveProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildSimpleWeaponsProficiency(ProficiencyLevels::TRAINED));
        character->AddProficiency(ProficiencyFactory::BuildMartialWeaponsProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildUnarmedCombatProficiency(ProficiencyLevels::APPRENTICE));
        break;
    case CharacterClasses::CLERIC:
        character->abilities.SetScore(AbilityTypes::WISDOM, abilityScores[0]);
        character->abilities.SetScore(AbilityTypes::STRENGTH, abilityScores[1]);
        character->abilities.SetScore(AbilityTypes::CONSTITUTION, abilityScores[2]);
        character->abilities.SetScore(AbilityTypes::CHARISMA, abilityScores[3]);
        character->abilities.SetScore(AbilityTypes::INTELLIGENCE, abilityScores[4]);
        character->abilities.SetScore(AbilityTypes::DEXTERITY, abilityScores[5]);

        character->hitDiceCount = 8;
        character->armorClass = 13;

        character->abilities.SetAttackAbility(AbilityTypes::STRENGTH);
        character->abilities.SetSpellCastingAbility(AbilityTypes::WISDOM);

        character->actions.Initialize(5);
        *character->actions.Allocate() = {CharacterActions::ATTACK};
        *character->actions.Allocate() = {CharacterActions::CAST_SPELL};
        *character->actions.Allocate() = {CharacterActions::DODGE};
        *character->actions.Allocate() = {CharacterActions::DASH};
        *character->actions.Allocate() = {CharacterActions::HELP};

        character->AddWeapon(WeaponFactory::BuildMace());

        character->AddSpell(SpellFactory::BuildCureWounds());
        character->AddSpell(SpellFactory::BuildSacredFlame());

        character->AddProficiency(ProficiencyFactory::BuildPerceptionProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildReflexSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildFortitudeSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildWillSaveProficiency(ProficiencyLevels::TRAINED));

        character->AddProficiency(ProficiencyFactory::BuildSimpleWeaponsProficiency(ProficiencyLevels::TRAINED));
        character->AddProficiency(ProficiencyFactory::BuildUnarmedCombatProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildDivineMagicProficiency(ProficiencyLevels::TRAINED));
        break;
    case CharacterClasses::WIZARD:
        character->abilities.SetScore(AbilityTypes::INTELLIGENCE, abilityScores[0]);
        character->abilities.SetScore(AbilityTypes::CONSTITUTION, abilityScores[1]);
        character->abilities.SetScore(AbilityTypes::DEXTERITY, abilityScores[2]);
        character->abilities.SetScore(AbilityTypes::WISDOM, abilityScores[3]);
        character->abilities.SetScore(AbilityTypes::CHARISMA, abilityScores[4]);
        character->abilities.SetScore(AbilityTypes::STRENGTH, abilityScores[5]);

        character->hitDiceCount = 6;
        character->armorClass = 11;

        character->abilities.SetAttackAbility(AbilityTypes::STRENGTH);
        character->abilities.SetSpellCastingAbility(AbilityTypes::INTELLIGENCE);

        character->actions.Initialize(4);
        *character->actions.Allocate() = {CharacterActions::ATTACK};
        *character->actions.Allocate() = {CharacterActions::CAST_SPELL};
        *character->actions.Allocate() = {CharacterActions::DODGE};
        *character->actions.Allocate() = {CharacterActions::DASH};

        character->AddWeapon(WeaponFactory::BuildStaff());

        character->AddSpell(SpellFactory::BuildFrostRay());
        character->AddSpell(SpellFactory::BuildShockingGrasp());
        character->AddSpell(SpellFactory::BuildMagicMissile());

        character->AddProficiency(ProficiencyFactory::BuildPerceptionProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildReflexSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildFortitudeSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildWillSaveProficiency(ProficiencyLevels::TRAINED));

        character->AddProficiency(ProficiencyFactory::BuildSimpleWeaponsProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildArcaneMagicProficiency(ProficiencyLevels::TRAINED));
        break;
    }

    character->race = race;
    for(auto boost : race->Boosts)
    {
        character->BoostAbility(boost.Type, boost.Boost);
    }

    character->maximumHitPoints = character->hitDiceCount;
    character->maximumHitPoints += character->abilities.GetModifier(AbilityTypes::CONSTITUTION) * character->level;
    character->maximumHitPoints += race->HitPointBonus;

    auto battleScene = BattleScene::Get();
    character->tile = group.tile->GetEmptyTileInRange(5);
    character->tile->Character = character;

    character->defaultSpeed = 5;//utility::GetRandom(4, 6);

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