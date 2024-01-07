#pragma once

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/Battle/CharacterActionData.h"
#include "FlumenBattle/World/Character/Proficiency.h"
#include "FlumenBattle/World/Character/ProficiencyHandler.h"
#include "FlumenBattle/World/Character/Ability.h"
#include "FlumenBattle/World/Character/AbilityHandler.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/World/Character/Condition.h"
#include "FlumenBattle/World/Character/Modifier.h"

struct Weapon;
struct Spell;
struct SpellSlot;

namespace render
{
    class Texture;
}

namespace battle
{
    class Combatant;
    class BattleTile;
    class BattleInterface;
    class ArtificialController;
    class HumanController;
    class CharacterInfo;
    class CharacterDetailPanel;
    class SpellCaster;
}

namespace world::group
{
    class Group;
}

namespace world
{
    namespace character
    {
        class CharacterAllocator;
        struct CharacterAction;
        class CharacterClass;
        class Condition;
        class CharacterFactory;
        class Modifier;
    }
}

namespace world::character
{
    class Character
    {
    private:
        friend class container::Pool <Character>;

        friend class CharacterFactory;

        friend class battle::CharacterInfo;

        friend class battle::CharacterDetailPanel;

        friend class battle::BattleInterface;

        friend class Condition; 

        friend class ModifierAccessor;

        friend class battle::SpellCaster;

        friend class battle::ArtificialController;

        friend class battle::HumanController;

        friend class ProficiencyHandler;

        friend class AbilityHandler;

        friend class battle::Combatant;

        friend class CharacterAllocator;

        world::group::Group *group;

        const CharacterClass *type;

        const Race *race;

        const render::Texture *avatar;

        Word name;

        Integer level;

        AbilityHandler abilities;

        ProficiencyHandler proficiencies;

        Integer currentHitPoints;

        Integer maximumHitPoints;

        Integer defaultSpeed; //Combatant

        world::character::CharacterAction *selectedAction;

        Pool <Weapon> weapons;

        Weapon *selectedWeapon; //Combatant 

        Pool <Spell> spells;

        Spell *selectedSpell; //Combatant

        Pool <SpellSlot> spellSlots;

        battle::Combatant *combatant;

        ConditionManager conditions;

        ModifierManager modifiers;

        void Initialize();

        Integer GetActionRange() const;

        void AddWeapon(Weapon);

        void AddSpell(Spell);

        void AddProficiency(Proficiency);

        void AddModifier(Modifier modifier) {modifiers.AddModifier(modifier);}

    public:
        Character();

        bool IsAlive() const;

        Float GetHealth() const {return (Float)currentHitPoints / (Float)maximumHitPoints;}

        Integer GetCurrentHitPoints() const {return currentHitPoints;}

        Integer GetMaximumHitPoints() const {return maximumHitPoints;}

        Word GetClassName();

        const Race * GetRace() const {return race;}

        const CharacterClass * GetClass() const {return type;}

        world::group::Group * GetGroup() const {return group;}

        const render::Texture *GetAvatar() const {return avatar;}

        const Word GetName() const {return name;}

        const Ability & GetAbility(AbilityTypes type) const {return *abilities.GetAbility(type);}

        const Ability & GetAttackAbility() {return *abilities.GetAttackAbility();}

        const Ability & GetSpellCastingAbility() {return *abilities.GetSpellCastingAbility();}

        void BoostAbility(AbilityTypes, Integer);

        Integer GetMagicProficiencyBonus() {return proficiencies.GetMagicBonus(*this);}

        Integer GetPerceptionProficiencyBonus() {return proficiencies.GetPerceptionBonus(*this);}

        Integer GetReflexSaveBonus();

        Integer GetFortitudeSaveBonus();

        Integer GetWillSaveBonus();

        Integer GetSkillProficiency(SkillTypes skill) {return proficiencies.GetSkillBonus(*this, skill);}

        const Array <world::character::CharacterAction> &GetActions();

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

        bool HasCondition(Conditions condition) {return conditions.HasCondition(condition);}

        void TakeShortRest();

        void TakeLongRest();

        void AddCondition(ConditionData);

        void Update();
    };
}