#include "CharacterEssence.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/ClassFactory.h"
#include "FlumenBattle/Config.h"

using namespace world::group;

void CharacterEssence::Initialize(character::CharacterClasses newClass)
{
    isFunctioning = true;
    isAlive = true;
    characterClass = newClass;
    experience = 0;
    level = 1;

    ResetHitpoints();
}

bool CharacterEssence::SufferDamage(int damage)
{
    bool hasDied = false;

    health -= damage;
    if(health <= 0)
    {
        if(isAlive == true)
        {
            hasDied = true;
        }

        isAlive = false;
    }

    return hasDied;
}

int CharacterEssence::GetMaximumHitpoints() const
{
    auto &classType = character::ClassFactory::BuildClass(characterClass);
    return 40 + level * 2 + classType.HitDice;
}

void CharacterEssence::ResetHitpoints() 
{
    this->health = GetMaximumHitpoints();
}

void CharacterEssence::GainExperience(int amount)
{
    static const auto MAXIMUM_CHARACTER_LEVEL = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_CHARACTER_LEVEL).Integer;

    static const auto EXPERIENCE_THRESHOLDS = character::Character::GetExperienceThresholds();

    if(level == MAXIMUM_CHARACTER_LEVEL)
        return;

    experience += amount;

    while(true)
    {
        if(experience >= EXPERIENCE_THRESHOLDS[level])
        {
            experience -= EXPERIENCE_THRESHOLDS[level];

            level++;
        }
        else
        {
            break;
        }

        if(level == MAXIMUM_CHARACTER_LEVEL)
            return;
    }
}