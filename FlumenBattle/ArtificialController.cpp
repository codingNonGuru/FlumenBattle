#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Core/TaskManager.hpp"

#include "FlumenBattle/ArtificialController.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleTile.h"
#include "FlumenBattle/Weapon.h"
#include "FlumenBattle/Spell.h"

enum class ActionTriggers
{
    TARGET_DAMAGED, NONE, ALL
};

struct CombatantData
{
    Character *Character;

    Integer Distance;
};

struct CombatTarget
{
    CombatantData *Target;

    ActionTriggers Trigger;

    CharacterActions Action;

    WeaponTypes WeaponType;

    SpellTypes SpellType;

    CombatTarget() {}

    CombatTarget(CombatantData *target, ActionTriggers trigger, WeaponTypes weaponType) :
        Target(target), Trigger(trigger), WeaponType(weaponType), Action(CharacterActions::ATTACK) {}

    CombatTarget(CombatantData *target, ActionTriggers trigger, SpellTypes spellType) :
        Target(target), Trigger(trigger), SpellType(spellType), Action(CharacterActions::CAST_SPELL) {}

    bool IsTriggerValid()
    {
        switch(Trigger)
        {
            case ActionTriggers::TARGET_DAMAGED:
                return Target->Character->GetHealth() < 0.5f;
                break;
            case ActionTriggers::NONE:
                return true;
                break;
            case ActionTriggers::ALL:
                return false;
                break;
        }
    }
};

struct TileMoveData
{
    BattleTile *Tile;

    Integer Distance;

    bool operator < (const TileMoveData &other) {return this->Distance < other.Distance;}
};

Array <TileMoveData> tileMoveDatas = Array <TileMoveData> (64);

bool hasActed = false;

CombatantData enemyData;

CombatantData allyData;

CombatantData selfData;

CombatTarget primaryTarget;

CombatTarget opportunityTarget;

CombatantData ArtificialController::FindClosestEnemy()
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    Character *closestCharacter = nullptr;
    Integer closestDistance = INT_MAX;

    auto &characters = battleController->playerControlledGroup->GetCharacters();
    for(auto character = characters.GetStart(); character != characters.GetEnd(); ++character)
    {
        if(character->IsAlive() == false)
            continue;

        auto distance = selectedCharacter->GetDistanceTo(character);
        if(distance < closestDistance)
        {
            closestCharacter = character;
            closestDistance = distance;
        }
    }

    return {closestCharacter, closestDistance};
}

CombatantData ArtificialController::FindClosestAlly()
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    Character *closestCharacter = nullptr;
    Integer closestDistance = INT_MAX;

    auto &characters = battleController->computerControlledGroup->GetCharacters();
    for(auto character = characters.GetStart(); character != characters.GetEnd(); ++character)
    {
        if(character == selectedCharacter)
            continue;

        if(character->IsAlive() == false)
            continue;

        if(character->type != CharacterClasses::FIGHTER)
            continue;

        auto distance = selectedCharacter->GetDistanceTo(character);
        if(distance < closestDistance)
        {
            closestCharacter = character;
            closestDistance = distance;
        }
    }

    return {closestCharacter, closestDistance};
}

void ArtificialController::DetermineActionCourse()
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    enemyData = FindClosestEnemy();
    
    allyData = FindClosestAlly();

    selfData = {selectedCharacter, 0};

    Integer highestDamage;
    Weapon *strongestWeapon;

    switch(selectedCharacter->type)
    {
        case CharacterClasses::FIGHTER:
            selectedCharacter->SelectAction(CharacterActions::ATTACK);

            highestDamage = 0;
            strongestWeapon = nullptr;
            for(auto weapon = selectedCharacter->weapons.GetStart(); weapon != selectedCharacter->weapons.GetEnd(); ++weapon)
            {
                if(weapon->GetAverageDamage() > highestDamage)
                {
                    highestDamage = weapon->GetAverageDamage();
                    strongestWeapon = weapon;
                }
            }

            selectedCharacter->SelectWeapon(strongestWeapon);

            primaryTarget = {&enemyData, ActionTriggers::NONE, strongestWeapon->Type};
            opportunityTarget = primaryTarget;
            break;
        case CharacterClasses::RANGER:
            selectedCharacter->SelectAction(CharacterActions::ATTACK);

            highestDamage = 0;
            strongestWeapon = nullptr;
            for(auto weapon = selectedCharacter->weapons.GetStart(); weapon != selectedCharacter->weapons.GetEnd(); ++weapon)
            {
                if(weapon->GetAverageDamage() > highestDamage && weapon->Range > 1)
                {
                    highestDamage = weapon->GetAverageDamage();
                    strongestWeapon = weapon;
                }
            }

            selectedCharacter->SelectWeapon(strongestWeapon);

            primaryTarget = {&enemyData, ActionTriggers::NONE, strongestWeapon->Type};
            opportunityTarget = primaryTarget;
            break;
        case CharacterClasses::CLERIC:
            if(allyData.Character != nullptr)
            {
                selectedCharacter->SelectAction(CharacterActions::CAST_SPELL);
                selectedCharacter->SelectSpell(SpellTypes::CURE_WOUNDS);

                primaryTarget = {&allyData, ActionTriggers::TARGET_DAMAGED, SpellTypes::CURE_WOUNDS};

                opportunityTarget = {&enemyData, ActionTriggers::NONE, SpellTypes::SACRED_FLAME};
            }
            else
            {
                selectedCharacter->SelectAction(CharacterActions::CAST_SPELL);
                selectedCharacter->SelectSpell(SpellTypes::SACRED_FLAME);
                
                primaryTarget = {&enemyData, ActionTriggers::NONE, SpellTypes::SACRED_FLAME};

                opportunityTarget = {&selfData, ActionTriggers::TARGET_DAMAGED, SpellTypes::CURE_WOUNDS};
            }
            break;
        case CharacterClasses::WIZARD:
            selectedCharacter->SelectAction(CharacterActions::CAST_SPELL);
            selectedCharacter->SelectSpell(SpellTypes::FROST_RAY);

            primaryTarget = {&enemyData, ActionTriggers::NONE, SpellTypes::FROST_RAY};
            opportunityTarget = primaryTarget;
            break;
    }
}

void ArtificialController::MoveCharacter()
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    if(primaryTarget.Target->Character != nullptr)
    {
        BattleTile *targetedTile = nullptr;
        Integer closestDistance = INT_MAX;

        auto &nearbyTiles = selectedCharacter->GetTile()->GetNearbyTiles(1);
        for(auto tileIterator = nearbyTiles.GetStart(); tileIterator != nearbyTiles.GetEnd(); ++tileIterator)
        {
            auto tile = *tileIterator;
            if(tile->Character != nullptr)
                continue;

            Integer distance = selectedCharacter->GetTile()->GetDistanceTo(*tile);
            distance += tile->GetDistanceTo(*primaryTarget.Target->Character->GetTile());

            *tileMoveDatas.Allocate() = {tile, distance};
        }

        tileMoveDatas.SortAscendantly();

        closestDistance = tileMoveDatas.GetStart()->Distance;
        Index dataIndex = 0;
        for(auto data = tileMoveDatas.GetStart(); data != tileMoveDatas.GetEnd(); ++data, ++dataIndex)
        {
            if(data->Distance > closestDistance)
            {
                break;
            }
        }

        dataIndex = utility::GetRandom(0, dataIndex - 1);
        targetedTile = tileMoveDatas.Get(dataIndex)->Tile;

        battleController->TargetTile(targetedTile);
        battleController->Move();

        tileMoveDatas.Reset();

        primaryTarget.Target->Distance = selectedCharacter->GetDistanceTo(primaryTarget.Target->Character);
    }
}

void ArtificialController::PerformAction()
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    auto movement = selectedCharacter->GetMovement();
    bool isWithinRange = selectedCharacter->GetActionRange() >= primaryTarget.Target->Distance;
    
    if(movement > 0 && isWithinRange == false) 
    {
        MoveCharacter();

        TaskManager::Add()->Initialize(this, &ArtificialController::PerformAction, 0.1f);
    }
    else if(isWithinRange == true)
    {
        if(primaryTarget.IsTriggerValid())
        {
            battleController->TargetCharacter(primaryTarget.Target->Character);
            battleController->Act();
        }
        else if(CheckOpportunity())
        {}

        hasActed = true;   
    }
    else
    {
        if(CheckOpportunity())
        {}
        else
        {
            selectedCharacter->SelectAction(CharacterActions::DODGE);
            battleController->Act();
        }

        TaskManager::Add()->Initialize(battleController, &BattleController::EndTurn, 0.7f);
    }        

    if(hasActed)
    {
        TaskManager::Add()->Initialize(battleController, &BattleController::EndTurn, 0.7f);
    }
}

bool ArtificialController::CheckOpportunity()
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    if(opportunityTarget.IsTriggerValid())
    {
        selectedCharacter->SelectAction(opportunityTarget.Action);

        if(opportunityTarget.Action == CharacterActions::ATTACK)
        {
            selectedCharacter->SelectWeapon(opportunityTarget.WeaponType);
        }
        else
        {
            selectedCharacter->SelectSpell(opportunityTarget.SpellType);
        }

        bool isWithinRange = selectedCharacter->GetActionRange() >= opportunityTarget.Target->Distance;  

        if(isWithinRange)
        {
            battleController->TargetCharacter(opportunityTarget.Target->Character);
            battleController->Act();

            return true;
        }
    }

    return false;
}

void ArtificialController::UpdateCharacter()
{
    hasActed = false;

    DetermineActionCourse();

    PerformAction();
}
