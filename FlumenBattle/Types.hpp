#pragma once

#include "FlumenEngine/Core/Types.hpp"
#include "FlumenCore/Conventions.hpp"

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

enum class CharacterRaces {DWARF, GNOME, HUMAN, HALFLING, ELF, ORC, GOBLIN};

enum class CharacterActions {ATTACK, CAST_SPELL, DASH, DISENGAGE, DODGE, HELP, HIDE, READY, SEARCH, USE_OBJECT};

enum class WeaponTypes {SHORT_SWORD, GREAT_SWORD, HAND_AXE, GREAT_AXE, SHORT_BOW, LONG_BOW, STAFF, MACE};

enum class SpellTypes {MAGIC_MISSILE, FROST_RAY, SHOCKING_GRASP, CURE_WOUNDS};

enum class ConditionTypes {CHILL, PARALYSIS, CHARM, EVASION, NONE};