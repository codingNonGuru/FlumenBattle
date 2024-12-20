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

namespace utility
{
    struct RollMaterial;
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
        struct Item;
        struct Spell;
        struct SpellSlot;
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

        const ::render::Texture *avatar;

        Word name;

        Integer level;

        Integer experience;

        AbilityHandler abilities;

        ProficiencyHandler proficiencies;

        Integer currentHitPoints;

        Integer maximumHitPoints;

        Integer defaultSpeed; //Combatant

        world::character::CharacterAction *selectedAction; 

        Spell *selectedSpell; //Combatant

        int spellUseCount;

        //container::Block <SpellSlot, 6> spellsSlots;

        battle::Combatant *combatant;

        ConditionManager conditions;

        ModifierManager modifiers;

        container::Block <Item *, (int)ItemPositions::COUNT> items {nullptr};

        void Initialize();

        void Setup();

        void AddProficiency(Proficiency);

        void AddModifier(Modifier modifier) {modifiers.AddModifier(modifier);}

    public:
        Character();

        bool IsAlive() const;

        Float GetHealth() const {return (Float)currentHitPoints / (Float)maximumHitPoints;}

        const Integer &GetCurrentHitPoints() const {return currentHitPoints;}

        const Integer &GetMaximumHitPoints() const {return maximumHitPoints;}

        const Word &GetClassName() const;

        const Race * GetRace() const {return race;}

        const CharacterClass * GetClass() const {return type;}

        world::group::Group * GetGroup() const {return group;}

        const ::render::Texture *GetAvatar() const {return avatar;}

        const Word GetName() const {return name;}

        Integer GetLevel() const {return level;}

        float GetLevelProgress() const;

        const Ability & GetAbility(AbilityTypes type) const {return *abilities.GetAbility(type);}

        const Ability & GetAttackAbility() {return *abilities.GetAttackAbility();}

        const Ability & GetSpellCastingAbility() {return *abilities.GetSpellCastingAbility(*this);}

        void BoostAbility(AbilityTypes, Integer);

        Integer GetMagicProficiencyBonus() {return proficiencies.GetMagicBonus(*this);}

        //Integer GetPerceptionProficiencyBonus() {return proficiencies.GetPerceptionBonus(*this);}

        Integer GetReflexSaveBonus();

        Integer GetFortitudeSaveBonus();

        Integer GetWillSaveBonus();

        Integer GetSkillProficiency(SkillTypes skill) {return proficiencies.GetSkillBonus(*this, skill);}

        bool IsTrainedInSkill(SkillTypes) const;

        Word GetSkillTexture(SkillTypes) const;

        const Array <world::character::CharacterAction> &GetActions();

        Integer GetActionRange() const;

        bool CanCastSpells() const;

        const Array <Spell> &GetSpells() const;

        Integer GetArmorClass() const;

        Integer GetAttackRating() const;

        Integer GetSpellPower() const;

        utility::RollMaterial GetDamage() const;

        const Pool <Condition> &GetConditions() const;

        int GetInitiativeModifier() const;

        bool SelectAction(Index);

        bool SelectActionOption(Index);

        bool SelectAction(CharacterActions);

        bool SelectWeapon(Weapon *);

        bool SelectWeapon(WeaponTypes);

        bool SelectSpell(Spell *);

        bool SelectSpell(SpellTypes);

        void EquipItem(Item *, ItemPositions);

        void UnequipItem(ItemPositions);

        Index GetSelectedActionIndex() const;

        Index GetSelectedSubactionIndex() const;

        world::character::CharacterAction* GetSelectedAction() const;

        Spell* GetSelectedSpell() const {return selectedSpell;}

        Item *GetItem(ItemPositions position) const {return *items.Get((int)position);}

        bool HasCondition(Conditions condition) {return conditions.HasCondition(condition);}

        int GetModifier(Modifiers modifier) {return modifiers.GetAmount(modifier);}

        void TakeShortRest();

        void TakeLongRest();

        void AddCondition(ConditionData);

        void RemoveCondition(Conditions);

        void Update();

        void RefreshModifiers();

        void SufferDamage(int);

        void GainExperience(int);
    };
}