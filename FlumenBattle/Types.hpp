#pragma once

#include "FlumenEngine/Core/Types.hpp"
#include "FlumenCore/Conventions.hpp"

#define ABILITY_COUNT 6

enum class Scenes
{
	BATTLE
};

class FrameBuffers
{
public:
	static Word const DEFAULT;
};

class Elements
{
public:
	static Word const MAIN_MENU;
};

class Cameras
{
public:
	static Word const BATTLE;
};

typedef float Weight;

typedef int People;

typedef float Money;

typedef float Labor;

enum class CharacterClasses {FIGHTER, RANGER, CLERIC, WIZARD};

enum class RaceTypes {DWARF, GNOME, HUMAN, HALFLING, ELF, ORC, GOBLIN};

enum class CharacterActions {ATTACK, CAST_SPELL, DASH, DISENGAGE, DODGE, HELP, HIDE, READY, SEARCH, USE_OBJECT, MOVE};

enum class WeaponTypes {SHORT_SWORD, GREAT_SWORD, HAND_AXE, GREAT_AXE, SHORT_BOW, LONG_BOW, STAFF, MACE};

enum class SpellTypes {
	FROST_RAY, SHOCKING_GRASP, SACRED_FLAME, 
	CURE_WOUNDS, MAGIC_MISSILE, HEALING_WORD
	};

enum class ConditionTypes {CHILL, PARALYSIS, CHARM, EVASION, NONE};

enum class SavingThrows
{
	REFLEX_SAVE, FORTITUDE_SAVE, WILL_SAVE
};

enum class WeaponClasses
{
    SIMPLE_WEAPONS, MARTIAL_WEAPONS, ADVANCED_WEAPONS, UNARMED_COMBAT
};

enum class MagicTraditions
{
    ARCANE_MAGIC, DIVINE_MAGIC, OCCULT_MAGIC, PRIMAL_MAGIC
};

enum class ArmorClasses
{
    LIGHT_ARMOR, MEDIUM_ARMOR, HEAVY_ARMOR, UNARMORED
};

enum class SkillTypes
{
	ACROBATICS, ATHLETICS, CRAFTING, DECEPTION, LORE, PERFORMANCE, STEALTH
};

enum class AbilityTypes
{
    STRENGTH, DEXTERITY, CONSTITUTION, INTELLIGENCE, WISDOM, CHARISMA
};


