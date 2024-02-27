#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Render/TextureManager.hpp"

#include "FlumenBattle/World/Character/CharacterFactory.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Character/CharacterAllocator.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/World/Character/Weapon.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/World/Character/ProficiencyFactory.h"
#include "FlumenBattle/RaceFactory.h"
#include "FlumenBattle/World/Character/ClassFactory.h"
#include "FlumenBattle/World/Character/CharacterAction.h"
#include "FlumenBattle/World/Character/NameGenerator.h"
#include "FlumenBattle/World/Character/RecruitData.h"

using namespace world::character;

Integer abilityScores[ABILITY_COUNT];

int textureIndex = 0;

const Array <Word> avatarTextures = {"Icons_01", "Icons_02", "Icons_03", "Icons_13", "Icons_14", "Icons_15", "Icons_16", "Icons_17"};

Character *CharacterFactory::Create(const RecruitData &recruitData, world::group::Group &group)
{
    return Create(recruitData.Race, &ClassFactory::BuildClass(recruitData.Class), group, recruitData.IconTextureId);
}

Character *CharacterFactory::Create(const Race *race, const CharacterClass *type, world::group::Group& group, int iconId)
{
    auto character = world::character::CharacterAllocator::Get()->Allocate(group);

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

        /**character->actions.Allocate() = {CharacterActions::ATTACK};
        *character->actions.Allocate() = {CharacterActions::DODGE};
        *character->actions.Allocate() = {CharacterActions::DASH};*/

        //character->AddWeapon(WeaponFactory::BuildGreatSword());
        //character->AddWeapon(WeaponFactory::BuildHatchet());

        //character->AddProficiency(ProficiencyFactory::BuildPerceptionProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildReflexSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildFortitudeSaveProficiency(ProficiencyLevels::TRAINED));
        character->AddProficiency(ProficiencyFactory::BuildWillSaveProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildSimpleWeaponsProficiency(ProficiencyLevels::TRAINED));
        character->AddProficiency(ProficiencyFactory::BuildMartialWeaponsProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildUnarmedCombatProficiency(ProficiencyLevels::TRAINED));

        character->AddProficiency(ProficiencyFactory::BuildSkillProficiency(SkillTypes::PERCEPTION, ProficiencyLevels::APPRENTICE));
        break;
    case CharacterClasses::RANGER:
        character->abilities.SetScore(AbilityTypes::DEXTERITY, abilityScores[0]);
        character->abilities.SetScore(AbilityTypes::WISDOM, abilityScores[1]);
        character->abilities.SetScore(AbilityTypes::CONSTITUTION, abilityScores[2]);
        character->abilities.SetScore(AbilityTypes::INTELLIGENCE, abilityScores[3]);
        character->abilities.SetScore(AbilityTypes::CHARISMA, abilityScores[4]);
        character->abilities.SetScore(AbilityTypes::STRENGTH, abilityScores[5]);

        character->abilities.SetAttackAbility(AbilityTypes::DEXTERITY);

        /**character->actions.Allocate() = {CharacterActions::ATTACK};
        *character->actions.Allocate() = {CharacterActions::DODGE};
        *character->actions.Allocate() = {CharacterActions::DASH};*/

        //character->AddWeapon(WeaponFactory::BuildLongBow());
        //character->AddWeapon(WeaponFactory::BuildShortSword());

        //character->AddProficiency(ProficiencyFactory::BuildPerceptionProficiency(ProficiencyLevels::TRAINED));

        character->AddProficiency(ProficiencyFactory::BuildReflexSaveProficiency(ProficiencyLevels::TRAINED));
        character->AddProficiency(ProficiencyFactory::BuildFortitudeSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildWillSaveProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildSimpleWeaponsProficiency(ProficiencyLevels::TRAINED));
        character->AddProficiency(ProficiencyFactory::BuildMartialWeaponsProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildUnarmedCombatProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildSkillProficiency(SkillTypes::SURVIVAL, ProficiencyLevels::TRAINED));
        character->AddProficiency(ProficiencyFactory::BuildSkillProficiency(SkillTypes::PERCEPTION, ProficiencyLevels::APPRENTICE));
        break;
    case CharacterClasses::CLERIC:
        character->abilities.SetScore(AbilityTypes::WISDOM, abilityScores[0]);
        character->abilities.SetScore(AbilityTypes::STRENGTH, abilityScores[1]);
        character->abilities.SetScore(AbilityTypes::CONSTITUTION, abilityScores[2]);
        character->abilities.SetScore(AbilityTypes::CHARISMA, abilityScores[3]);
        character->abilities.SetScore(AbilityTypes::INTELLIGENCE, abilityScores[4]);
        character->abilities.SetScore(AbilityTypes::DEXTERITY, abilityScores[5]);

        character->abilities.SetAttackAbility(AbilityTypes::STRENGTH);

        /**character->actions.Allocate() = {CharacterActions::ATTACK};
        *character->actions.Allocate() = {CharacterActions::CAST_SPELL};
        *character->actions.Allocate() = {CharacterActions::DODGE};
        *character->actions.Allocate() = {CharacterActions::DASH};
        *character->actions.Allocate() = {CharacterActions::HELP};*/

        //character->AddWeapon(WeaponFactory::BuildMace());

        //character->AddProficiency(ProficiencyFactory::BuildPerceptionProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildReflexSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildFortitudeSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildWillSaveProficiency(ProficiencyLevels::TRAINED));

        character->AddProficiency(ProficiencyFactory::BuildSimpleWeaponsProficiency(ProficiencyLevels::TRAINED));
        character->AddProficiency(ProficiencyFactory::BuildUnarmedCombatProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildDivineMagicProficiency(ProficiencyLevels::TRAINED));

        character->AddProficiency(ProficiencyFactory::BuildSkillProficiency(SkillTypes::PERCEPTION, ProficiencyLevels::APPRENTICE));
        break;
    case CharacterClasses::WIZARD:
        character->abilities.SetScore(AbilityTypes::INTELLIGENCE, abilityScores[0]);
        character->abilities.SetScore(AbilityTypes::CONSTITUTION, abilityScores[1]);
        character->abilities.SetScore(AbilityTypes::DEXTERITY, abilityScores[2]);
        character->abilities.SetScore(AbilityTypes::WISDOM, abilityScores[3]);
        character->abilities.SetScore(AbilityTypes::CHARISMA, abilityScores[4]);
        character->abilities.SetScore(AbilityTypes::STRENGTH, abilityScores[5]);

        character->abilities.SetAttackAbility(AbilityTypes::STRENGTH);

        /**character->actions.Allocate() = {CharacterActions::ATTACK};
        *character->actions.Allocate() = {CharacterActions::CAST_SPELL};
        *character->actions.Allocate() = {CharacterActions::DODGE};
        *character->actions.Allocate() = {CharacterActions::DASH};*/

        //character->AddWeapon(WeaponFactory::BuildStaff());

        //character->AddProficiency(ProficiencyFactory::BuildPerceptionProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildReflexSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildFortitudeSaveProficiency(ProficiencyLevels::APPRENTICE));
        character->AddProficiency(ProficiencyFactory::BuildWillSaveProficiency(ProficiencyLevels::TRAINED));

        character->AddProficiency(ProficiencyFactory::BuildSimpleWeaponsProficiency(ProficiencyLevels::APPRENTICE));

        character->AddProficiency(ProficiencyFactory::BuildArcaneMagicProficiency(ProficiencyLevels::TRAINED));

        character->AddProficiency(ProficiencyFactory::BuildSkillProficiency(SkillTypes::PERCEPTION, ProficiencyLevels::APPRENTICE));
        break;
    }

    character->race = race;
    for(auto boost : race->Boosts)
    {
        character->BoostAbility(boost.Type, boost.Boost);
    }

    character->name = NameGenerator::Get()->GenerateName();

    if(iconId == -1)
    {
        auto textureName = avatarTextures.Get(textureIndex++);
        character->avatar = render::TextureManager::GetTexture(*textureName);
        if(textureIndex == avatarTextures.GetSize())
        {
            textureIndex = 0;
        }
    }
    else
    {
        auto textureName = avatarTextures.Get(iconId);
        character->avatar = render::TextureManager::GetTexture(*textureName);
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

int CharacterFactory::GetIconCount()
{
    return avatarTextures.GetSize();
}

const char *CharacterFactory::GetTextureName(int id)
{
    return *avatarTextures.Get(id);
}