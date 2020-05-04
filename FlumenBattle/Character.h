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

class Character
{
public:
    struct Action
    {
        CharacterActions Type;

        Action() : Type() {}

        Action(CharacterActions type) : Type(type) {}

        bool operator== (const Action &other) {return this->Type == other.Type;}
    };

private:
    friend class container::Array<Character>;

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

    world::group::Group *group;

    const CharacterClass *type;

    const Race *race;

    Integer level;

    AbilityHandler abilities;

    ProficiencyHandler proficiencies;

    Integer currentHitPoints;

    Integer maximumHitPoints;

    Integer defaultSpeed; //Combatant

    Array <Action> actions;

    Action *selectedAction;

    Array <Weapon> weapons;

    Weapon *selectedWeapon; //Combatant 

    Array <Spell> spells;

    Spell *selectedSpell; //Combatant

    Array <SpellSlot> spellSlots;

    Combatant *combatant;

    Character();

    void Initialize();

    Integer GetActionRange() const;

    void AddWeapon(Weapon);

    void AddSpell(Spell);

    void AddProficiency(Proficiency);

public:
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

    Integer GetReflexSaveBonus();

    Integer GetFortitudeSaveBonus();

    Integer GetWillSaveBonus();

    Array <Action> & GetActions() {return actions;}

    const Array <Weapon> & GetWeapons() {return weapons;}

    const Array <Spell> & GetSpells() {return spells;}

    bool SelectAction(Index);

    bool SelectActionOption(Index);

    bool SelectAction(CharacterActions);

    bool SelectWeapon(Weapon *);

    bool SelectWeapon(WeaponTypes);

    bool SelectSpell(Spell *);

    bool SelectSpell(SpellTypes);

    Index GetSelectedActionIndex() const;

    Index GetSelectedSubactionIndex() const;

    Action* GetSelectedAction() const;

    Weapon* GetSelectedWeapon() const {return selectedWeapon;}

    Spell* GetSelectedSpell() const {return selectedSpell;}

    void TakeShortRest();

    void TakeLongRest();
};