#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/CharacterFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/CharacterAllocator.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/BattleTile.h"
#include "FlumenBattle/Weapon.h"
#include "FlumenBattle/Spell.h"
#include "FlumenBattle/ProficiencyFactory.h"
#include "FlumenBattle/RaceFactory.h"
#include "FlumenBattle/ClassFactory.h"
#include "FlumenBattle/World/Character/CharacterAction.h"

Integer abilityScores[ABILITY_COUNT];

Character* CharacterFactory::Create(const Race *race, const CharacterClass *type, world::group::Group& group)
{
    auto character = world::CharacterAllocator::Get()->Allocate(group);// group.GetCharacters().Add();

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

    switch(type->Class)
    {
    case CharacterClasses::FIGHTER:
        character->abilities.SetScore(AbilityTypes::STRENGTH, abilityScores[0]);
        character->abilities.SetScore(AbilityTypes::CONSTITUTION, abilityScores[1]);
        character->abilities.SetScore(AbilityTypes::DEXTERITY, abilityScores[2]);
        character->abilities.SetScore(AbilityTypes::WISDOM, abilityScores[3]);
        character->abilities.SetScore(AbilityTypes::CHARISMA, abilityScores[4]);
        character->abilities.SetScore(AbilityTypes::INTELLIGENCE, abilityScores[5]);

        character->abilities.SetAttackAbility(AbilityTypes::STRENGTH);

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

        character->abilities.SetAttackAbility(AbilityTypes::DEXTERITY);

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

        character->abilities.SetAttackAbility(AbilityTypes::STRENGTH);
        character->abilities.SetSpellCastingAbility(AbilityTypes::WISDOM);

        *character->actions.Allocate() = {CharacterActions::ATTACK};
        *character->actions.Allocate() = {CharacterActions::CAST_SPELL};
        *character->actions.Allocate() = {CharacterActions::DODGE};
        *character->actions.Allocate() = {CharacterActions::DASH};
        *character->actions.Allocate() = {CharacterActions::HELP};

        character->AddWeapon(WeaponFactory::BuildMace());

        character->AddSpell(SpellFactory::BuildCureWounds());
        character->AddSpell(SpellFactory::BuildSacredFlame());
        character->AddSpell(SpellFactory::BuildHealingWord());

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

        character->abilities.SetAttackAbility(AbilityTypes::STRENGTH);
        character->abilities.SetSpellCastingAbility(AbilityTypes::INTELLIGENCE);

        *character->actions.Allocate() = {CharacterActions::ATTACK};
        *character->actions.Allocate() = {CharacterActions::CAST_SPELL};
        *character->actions.Allocate() = {CharacterActions::DODGE};
        *character->actions.Allocate() = {CharacterActions::DASH};

        character->AddWeapon(WeaponFactory::BuildStaff());

        character->AddSpell(SpellFactory::BuildFrostRay());
        character->AddSpell(SpellFactory::BuildShockingGrasp());
        character->AddSpell(SpellFactory::BuildFireBolt());
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

    character->maximumHitPoints = type->HitDice;
    character->maximumHitPoints += character->abilities.GetModifier(AbilityTypes::CONSTITUTION) * character->level;
    character->maximumHitPoints += race->HitPointBonus;

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