#pragma once

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/CharacterActionData.h"
#include "FlumenBattle/Proficiency.h"
#include "FlumenBattle/ProficiencyHandler.h"
#include "FlumenBattle/Ability.h"
#include "FlumenBattle/AbilityHandler.h"
#include "FlumenBattle/Race.h"

class CharacterFactory;
class CharacterInfo;
class BattleTile;
struct Weapon;
struct Spell;
struct SpellSlot;
class Condition;
class Combatant;
class CharacterClass;
namespace world::group
{
    class Group;
}
namespace world
{
    class CharacterAllocator;
    namespace character
    {
        struct CharacterAction;
    }
}

class Character
{
private:
    friend class container::Pool <Character>;

    friend class CharacterFactory;

    friend class CharacterInfo;

    friend class CharacterDetailPanel;

    friend class BattleInterface;

    friend class Condition; 

    friend class SpellCaster;

    friend class ArtificialController;

    friend class HumanController;

    friend class ProficiencyHandler;

    friend class AbilityHandler;

    friend class Combatant;

    friend class world::CharacterAllocator;

    world::group::Group *group;

    const CharacterClass *type;

    const Race *race;

    Integer level;

    AbilityHandler abilities;

    ProficiencyHandler proficiencies;

    Integer currentHitPoints;

    Integer maximumHitPoints;

    Integer defaultSpeed; //Combatant

    Array <world::character::CharacterAction> actions;

    world::character::CharacterAction *selectedAction;

    Pool <Weapon> weapons;

    Weapon *selectedWeapon; //Combatant 

    Pool <Spell> spells;

    Spell *selectedSpell; //Combatant

    Pool <SpellSlot> spellSlots;

    Combatant *combatant;

    void Initialize();

    Integer GetActionRange() const;

    void AddWeapon(Weapon);

    void AddSpell(Spell);

    void AddProficiency(Proficiency);

public:
    Character();

    bool IsAlive() const;

    Float GetHealth() const {return (Float)currentHitPoints / (Float)maximumHitPoints;}

    Integer GetCurrentHitPoints() const {return currentHitPoints;}

    Integer GetMaximumHitPoints() const {return maximumHitPoints;}

    Word GetName();

    const Race * GetRace() const {return race;}

    const CharacterClass * GetClass() const {return type;}

    world::group::Group * GetGroup() const {return group;}

    const Ability & GetAbility(AbilityTypes type) const {return *abilities.GetAbility(type);}

    const Ability & GetAttackAbility() {return *abilities.GetAttackAbility();}

    const Ability & GetSpellCastingAbility() {return *abilities.GetSpellCastingAbility();}

    void BoostAbility(AbilityTypes, Integer);

    Integer GetMagicProficiencyBonus() {return proficiencies.GetMagicBonus(*this);}

    Integer GetPerceptionProficiencyBonus() {return proficiencies.GetPerceptionBonus(*this);}

    Integer GetReflexSaveBonus();

    Integer GetFortitudeSaveBonus();

    Integer GetWillSaveBonus();

    Array <world::character::CharacterAction> & GetActions() {return actions;}

    const Pool <Weapon> & GetWeapons() {return weapons;}

    const Pool <Spell> & GetSpells() {return spells;}

    bool SelectAction(Index);

    bool SelectActionOption(Index);

    bool SelectAction(CharacterActions);

    bool SelectWeapon(Weapon *);

    bool SelectWeapon(WeaponTypes);

    bool SelectSpell(Spell *);

    bool SelectSpell(SpellTypes);

    Index GetSelectedActionIndex() const;

    Index GetSelectedSubactionIndex() const;

    world::character::CharacterAction* GetSelectedAction() const;

    Weapon* GetSelectedWeapon() const {return selectedWeapon;}

    Spell* GetSelectedSpell() const {return selectedSpell;}

    void TakeShortRest();

    void TakeLongRest();
};