#pragma once

#include "FlumenEngine/Core/Types.hpp"
#include "FlumenCore/Conventions.hpp"

#define ABILITY_COUNT 6

#define BATTLE_ROUNDS_PER_TICK 100

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
	static Word const WORLD;
};

typedef float Weight;

typedef int People;

typedef float Money;

typedef float Labor;

enum class RaceTypes {DWARF, GNOME, HUMAN, HALFLING, ELF, ORC, GOBLIN};

enum class WeaponTypes {SHORT_SWORD, GREAT_SWORD, HAND_AXE, GREAT_AXE, SHORT_BOW, LONG_BOW, STAFF, MACE};

enum class SpellTypes {
	FROST_RAY, SHOCKING_GRASP, SACRED_FLAME, FIRE_BOLT, FIRE_BALL,
	CURE_WOUNDS, MAGIC_MISSILE, HEALING_WORD, BLESS
	};

enum class SavingThrows
{
	REFLEX, FORTITUDE, WILL
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

enum class TravelPenaltyTypes
{
	MOUNTAINS,
	WOODS,
	SEA
};

struct TravelPenalty
{
	Integer Value;

	container::SmartBlock <TravelPenaltyTypes, 4> Penalties;
};

