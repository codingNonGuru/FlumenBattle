#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/CharacterInfo.h"
#include "FlumenBattle/World/Character/Weapon.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/World/Character/Condition.h"
#include "FlumenBattle/Battle/SpellCaster.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/Character/CharacterAction.h"
#include "FlumenBattle/World/Character/ProficiencyFactory.h"

namespace world::character
{
    Character::Character()
    {
        //weapons.Initialize(4);
        //spells.Initialize(8);
        //spellSlots.Initialize(8);
    }

    void Character::Initialize()
    {
        selectedAction = type->Actions.GetStart();
        selectedSpell = nullptr;//spells.GetStart();

        //*spellSlots.Add() = {2};

        items = container::Block <Item *, (int)ItemPositions::COUNT> {nullptr};

        proficiencies.Initialize();

        //isFatigued = false;
    }

    void Character::Setup()
    {
        currentHitPoints = maximumHitPoints / 2;

        spellUseCount = 3;
    }

    bool Character::IsAlive() const
    {
        return currentHitPoints > 0;
    }

    void Character::AddProficiency(Proficiency proficiency)
    {
        proficiencies.AddProficiency(proficiency);
    }

    Integer Character::GetActionRange() const
    {
        if(IsAlive())
        {
            if(selectedAction->Type == CharacterActions::ATTACK)
            {
                auto mainHandItem = GetItem(ItemPositions::MAIN_HAND);
                if(mainHandItem == nullptr)
                    return 1;
                else if(mainHandItem->IsRangedWeapon() == true)
                    return 24;
                else
                    return 1;
            }
            else if(selectedAction->Type == CharacterActions::CAST_SPELL)
            {
                return selectedSpell->Range;
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }

    const Word &Character::GetClassName() const
    {
        return type->Name;
    }

    void Character::BoostAbility(AbilityTypes type, Integer boost)
    {
        abilities.Boost(type, boost);
    }

    Integer Character::GetReflexSaveBonus()
    {
        return proficiencies.GetSaveBonus(*this, SavingThrows::REFLEX) + abilities.GetModifier(AbilityTypes::DEXTERITY);
    }

    Integer Character::GetFortitudeSaveBonus()
    {
        return proficiencies.GetSaveBonus(*this, SavingThrows::FORTITUDE) + abilities.GetModifier(AbilityTypes::CONSTITUTION);
    }

    Integer Character::GetWillSaveBonus()
    {
        return proficiencies.GetSaveBonus(*this, SavingThrows::WILL) + abilities.GetModifier(AbilityTypes::WISDOM);
    }

    bool Character::IsTrainedInSkill(SkillTypes skill) const
    {
        return proficiencies.IsTrainedInSkill(*this, skill);
    }

    Word Character::GetSkillTexture(SkillTypes skill) const
    {
        return ProficiencyFactory::BuildSkillType(skill).TextureName;
    }

    const Array <world::character::CharacterAction> &Character::GetActions() 
    {
        return type->Actions;
    }

    bool Character::CanCastSpells() const
    {
        return type->CanCastSpells();
    }

    const Array <Spell> &Character::GetSpells() const
    {
        return type->SpellPower.Spells;
    }

    const Pool <Condition> &Character::GetConditions() const
    {
        return conditions.GetConditions();
    }

    int Character::GetInitiativeModifier() const
    {
        auto initiative = abilities.GetModifier(world::character::AbilityTypes::DEXTERITY);

        initiative += GetModifier(Modifiers::INITIATIVE_BONUS);

        initiative -= GetModifier(Modifiers::INITIATIVE_PENALTY);

        return initiative;
    }

    Integer Character::GetArmorClass() const
    {
        if(GetItem(ItemPositions::BODY) != nullptr)
        {
            auto armorClass = 10 + modifiers.GetAmount(Modifiers::BONUS_ARMOR_CLASS);

            auto dexterityLimit = modifiers.GetAmount(Modifiers::ARMOR_DEXTERITY_LIMIT);
            auto dexterityBonus = GetAbility(AbilityTypes::DEXTERITY).Modifier;
            if(dexterityBonus > dexterityLimit)
            {
                dexterityBonus = dexterityLimit;
            }

            return armorClass + dexterityBonus;
        }
        else
        {
            return 10 + GetAbility(AbilityTypes::DEXTERITY).Modifier;
        }
    }

    Integer Character::GetAttackRating() const
    {
        auto bonus = [&] 
        {
            auto mainHandItem = GetItem(ItemPositions::MAIN_HAND);
            if(mainHandItem != nullptr)
            {
                bool isRanged = mainHandItem->IsRangedWeapon();
                if(isRanged)
                {
                    return GetAbility(AbilityTypes::DEXTERITY).Modifier;
                }
                else
                {
                    return GetAbility(AbilityTypes::STRENGTH).Modifier;
                }
            }
            else
            {
                return GetAbility(AbilityTypes::STRENGTH).Modifier;
            }
        } ();

        return modifiers.GetAmount(Modifiers::ATTACK_RATING_BONUS) + bonus;
    }

    Integer Character::GetSpellPower() const
    {
        if(type->CanCastSpells() == false)
            return 0;

        auto abilityModifier = abilities.GetSpellCastingAbilityModifier(*this);
        auto proficiencyModifer = proficiencies.GetMagicBonus(*this);
        return abilityModifier + proficiencyModifer;
    }

    utility::RollMaterial Character::GetDamage() const
    {
        auto bonus = [&] 
        {
            auto mainHandItem = GetItem(ItemPositions::MAIN_HAND);
            if(mainHandItem != nullptr)
            {
                bool isRanged = mainHandItem->IsRangedWeapon();
                if(isRanged)
                {
                    return GetAbility(AbilityTypes::DEXTERITY).Modifier;
                }
                else
                {
                    return GetAbility(AbilityTypes::STRENGTH).Modifier;
                }
            }
            else
            {
                return GetAbility(AbilityTypes::STRENGTH).Modifier;
            }
        } ();

        bonus += modifiers.GetAmount(Modifiers::DAMAGE_BONUS);

        auto mainHandItem = GetItem(ItemPositions::MAIN_HAND);
        if(mainHandItem != nullptr)
        {
            return {(utility::RollDies)modifiers.GetAmount(Modifiers::BASE_ATTACK_DIE_TYPE), modifiers.GetAmount(Modifiers::BASE_ATTACK_DIE_COUNT), bonus};
        }
        else
        {
            return {utility::RollDies::D4, 1, bonus};
        }
    }

    bool Character::SelectAction(Index index)
    {
        if(index >= type->Actions.GetSize())
            return false;

        selectedAction = type->Actions.Get(index);
        return true;
    }

    bool Character::SelectActionOption(Index index)
    {
        switch(selectedAction->Type)
        {
            case CharacterActions::ATTACK:
                /*if(index >= weapons.GetSize())
                    return false;

                selectedWeapon = weapons.Get(index);*/
                return true;
            case CharacterActions::CAST_SPELL:
                if(index >= type->SpellPower.Spells.GetSize())
                    return false;

                selectedSpell = type->SpellPower.Spells.Get(index);
                return true;
            default:
                return false;
        }
    }

    bool Character::SelectAction(CharacterActions action)
    {
        selectedAction = type->Actions.Find(action);

        return true;
    }

    bool Character::SelectSpell(Spell *spell)
    {
        selectedSpell = spell;

        return true;
    }

    bool Character::SelectSpell(SpellTypes type)
    {
        selectedSpell = this->type->SpellPower.Spells.Find(type);

        return true;
    }

    void Character::EquipItem(Item *item, ItemPositions position) 
    {
        *items[(int)position] = item;

        item->IsUsed = true;

        RefreshModifiers();
    }

    void Character::UnequipItem(ItemPositions position) 
    {
        auto item = items[(int)position];

        (*item)->IsUsed = false;

        *item = nullptr;

        RefreshModifiers();
    }


    Index Character::GetSelectedActionIndex() const
    {
        return selectedAction - type->Actions.GetStart();
    }

    Index Character::GetSelectedSubactionIndex() const
    {
        switch(selectedAction->Type)
        {
            case CharacterActions::ATTACK:
                return 0;//weapons.GetIndex(selectedWeapon);//  selectedWeapon - weapons.GetStart();
            case CharacterActions::CAST_SPELL:
                return selectedSpell - type->SpellPower.Spells.GetStart();
            default:
                return 0;
        }
    }

    world::character::CharacterAction* Character::GetSelectedAction() const
    {
        return selectedAction;
    }

    void Character::TakeShortRest()
    {
        currentHitPoints += level;
        if(currentHitPoints > maximumHitPoints)
        {
            currentHitPoints = maximumHitPoints;
        }
    }

    void Character::TakeLongRest()
    {
        for(Integer i = 0; i < level; ++i)
        {
            currentHitPoints += utility::GetRandom(1, type->HitDice);
        }

        if(currentHitPoints > maximumHitPoints)
        {
            currentHitPoints = maximumHitPoints;
        }

        conditions.RemoveCondition(Conditions::FATIGUE);
    }

    void Character::AddCondition(ConditionData data)
    {
        conditions.AddCondition(data);
    }

    void Character::RemoveCondition(Conditions condition)
    {
        conditions.RemoveCondition(condition);
    }

    void Character::Update()
    {
        conditions.Update();

        RefreshModifiers();
    }

    void Character::RefreshModifiers()
    {
        modifiers.ClearModifiers();

        conditions.ApplyModifiers(*this);

        for(auto &item : items)
        {
            if(item == nullptr)
                continue;

            item->ApplyEffect(*this);
        }
    }

    void Character::SufferDamage(int damage)
    {
        currentHitPoints -= damage;
    }
}