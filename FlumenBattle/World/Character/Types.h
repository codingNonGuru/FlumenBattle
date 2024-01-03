#pragma once

namespace world::character
{
    enum class CharacterActions {ATTACK, CAST_SPELL, DASH, DISENGAGE, DODGE, HELP, HIDE, READY, SEARCH, USE_OBJECT, MOVE};

    enum class CharacterClasses {FIGHTER, RANGER, CLERIC, WIZARD};

    enum class Conditions {SICKENED, FRIGHTENED, PARALYZED, HOBBLED, EVASION, FATIGUE};

    enum class Modifiers {DAMAGE_INCREASE, ALL_ROLLS_PENALTY};
}