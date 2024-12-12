#pragma once

namespace world::character
{
    enum class CharacterActions {ATTACK, CAST_SPELL, DASH, DISENGAGE, DODGE, HELP, HIDE, READY, SEARCH, USE_OBJECT, MOVE};

    enum class CharacterClasses {FIGHTER, RANGER, CLERIC, WIZARD};

    enum class Conditions {SICKENED, FRIGHTENED, PARALYZED, HOBBLED, EVASION, FATIGUE, NOURISHED, BLESSED, WALL_PROTECTION, SURPRISED};

    enum class Modifiers 
    {
        BASE_ATTACK_DIE_TYPE, 
        BASE_ATTACK_DIE_COUNT, 
        BONUS_ARMOR_CLASS, 
        ARMOR_DEXTERITY_LIMIT, 
        ALL_ROLLS_PENALTY, 
        ATTACK_RATING_BONUS, 
        DAMAGE_BONUS, 
        FORTITUDE_BONUS, 
        REFLEX_BONUS, 
        MOVE_SPEED,
        INITIATIVE_BONUS,
        INITIATIVE_PENALTY
    };

    enum class ItemTypes {SWORD, AXE, SHIELD, ARMOR, HELMET, SPEAR, BOW, DAGGER, FOOD};

    enum class ItemPositions {HEAD, MAIN_HAND, OFF_HAND, BODY, FEET, POUCH, COUNT, NONE};

    enum class ItemUses {WEAPON, BODY_WEAR, HEAD_WEAR, SHIELD, POTION, SCROLL, OTHER};

    enum class AbilityTypes
    {
        STRENGTH, DEXTERITY, CONSTITUTION, INTELLIGENCE, WISDOM, CHARISMA
    };

    enum class CheckClasses
    {
        SAVE, /*PERCEPTION,*/ WEAPON, ARMOR, MAGIC, SKILL
    };

    enum class ProficiencyLevels
    {
        UNTRAINED, APPRENTICE, TRAINED, EXPERIENCED, MASTER, LEGENDARY
    };

    enum class SkillTypes
    {
        ACROBATICS, ATHLETICS, CRAFTING, DECEPTION, INTIMIDATION, LORE, PERFORMANCE, PERSUASION, STEALTH, SURVIVAL, PERCEPTION, NONE
    };
}