#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Core/TaskManager.hpp"

#include "FlumenBattle/ArtificialController.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleTile.h"
#include "FlumenBattle/Weapon.h"
#include "FlumenBattle/Spell.h"
#include "FlumenBattle/Combatant.h"
#include "FlumenBattle/CombatGroup.h"

enum class ActionTriggers
{
    TARGET_DAMAGED, NONE, ALL
};

struct CombatantData
{
    class Combatant *Combatant;

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
                return Target->Combatant->GetHealth() < 0.5f;
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

BattleController *battleController = nullptr;

BattleScene *battleScene = nullptr;

Combatant *selectedCombatant = nullptr;

CombatantData ArtificialController::FindClosestEnemy()
{
    Combatant *closestCharacter = nullptr;
    Integer closestDistance = INT_MAX;

    auto &combatants = battleScene->GetPlayerGroup()->GetCombatants();
    for(auto character = combatants.GetStart(); character != combatants.GetEnd(); ++character)
    {
        if(character->IsAlive() == false)
            continue;

        auto distance = selectedCombatant->GetDistanceTo(character);
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
    Combatant *closestCharacter = nullptr;
    Integer closestDistance = INT_MAX;

    auto &characters = battleScene->GetComputerGroup()->GetCombatants();
    for(auto combatant = characters.GetStart(); combatant != characters.GetEnd(); ++combatant)
    {
        if(combatant == selectedCombatant)
            continue;

        if(combatant->IsAlive() == false)
            continue;

        if(combatant->character->type != CharacterClasses::FIGHTER)
            continue;

        auto distance = selectedCombatant->GetDistanceTo(combatant);
        if(distance < closestDistance)
        {
            closestCharacter = combatant;
            closestDistance = distance;
        }
    }

    return {closestCharacter, closestDistance};
}

void ArtificialController::DetermineActionCourse()
{
    enemyData = FindClosestEnemy();
    
    allyData = FindClosestAlly();

    selfData = {selectedCombatant, 0};

    Integer highestDamage;
    Weapon *strongestWeapon;

    switch(selectedCombatant->character->type)
    {
        case CharacterClasses::FIGHTER:
            selectedCombatant->character->SelectAction(CharacterActions::ATTACK);

            highestDamage = 0;
            strongestWeapon = nullptr;
            for(auto weapon = selectedCombatant->character->weapons.GetStart(); weapon != selectedCombatant->character->weapons.GetEnd(); ++weapon)
            {
                if(weapon->GetAverageDamage() > highestDamage)
                {
                    highestDamage = weapon->GetAverageDamage();
                    strongestWeapon = weapon;
                }
            }

            selectedCombatant->character->SelectWeapon(strongestWeapon);

            primaryTarget = {&enemyData, ActionTriggers::NONE, strongestWeapon->Type};
            opportunityTarget = primaryTarget;
            break;
        case CharacterClasses::RANGER:
            selectedCombatant->character->SelectAction(CharacterActions::ATTACK);

            highestDamage = 0;
            strongestWeapon = nullptr;
            for(auto weapon = selectedCombatant->character->weapons.GetStart(); weapon != selectedCombatant->character->weapons.GetEnd(); ++weapon)
            {
                if(weapon->GetAverageDamage() > highestDamage && weapon->Range > 1)
                {
                    highestDamage = weapon->GetAverageDamage();
                    strongestWeapon = weapon;
                }
            }

            selectedCombatant->character->SelectWeapon(strongestWeapon);

            primaryTarget = {&enemyData, ActionTriggers::NONE, strongestWeapon->Type};
            opportunityTarget = primaryTarget;
            break;
        case CharacterClasses::CLERIC:
            if(allyData.Combatant != nullptr)
            {
                selectedCombatant->character->SelectAction(CharacterActions::CAST_SPELL);
                selectedCombatant->character->SelectSpell(SpellTypes::CURE_WOUNDS);

                primaryTarget = {&allyData, ActionTriggers::TARGET_DAMAGED, SpellTypes::CURE_WOUNDS};

                opportunityTarget = {&enemyData, ActionTriggers::NONE, SpellTypes::SACRED_FLAME};
            }
            else
            {
                selectedCombatant->character->SelectAction(CharacterActions::CAST_SPELL);
                selectedCombatant->character->SelectSpell(SpellTypes::SACRED_FLAME);
                
                primaryTarget = {&enemyData, ActionTriggers::NONE, SpellTypes::SACRED_FLAME};

                opportunityTarget = {&selfData, ActionTriggers::TARGET_DAMAGED, SpellTypes::CURE_WOUNDS};
            }
            break;
        case CharacterClasses::WIZARD:
            selectedCombatant->character->SelectAction(CharacterActions::CAST_SPELL);
            selectedCombatant->character->SelectSpell(SpellTypes::FROST_RAY);

            primaryTarget = {&enemyData, ActionTriggers::NONE, SpellTypes::FROST_RAY};
            opportunityTarget = primaryTarget;
            break;
    }
}

void ArtificialController::MoveCharacter()
{
    if(primaryTarget.Target->Combatant != nullptr)
    {
        BattleTile *targetedTile = nullptr;
        Integer closestDistance = INT_MAX;

        auto &nearbyTiles = selectedCombatant->tile->GetNearbyTiles(1);
        for(auto tileIterator = nearbyTiles.GetStart(); tileIterator != nearbyTiles.GetEnd(); ++tileIterator)
        {
            auto tile = *tileIterator;
            if(tile->Combatant != nullptr)
                continue;

            Integer distance = selectedCombatant->tile->GetDistanceTo(*tile);
            distance += tile->GetDistanceTo(*primaryTarget.Target->Combatant->tile);

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

        primaryTarget.Target->Distance = selectedCombatant->GetDistanceTo(primaryTarget.Target->Combatant);
    }
}

void ArtificialController::PerformAction()
{
    auto movement = selectedCombatant->GetMovement();
    bool isWithinRange = selectedCombatant->character->GetActionRange() >= primaryTarget.Target->Distance;
    
    if(movement > 0 && isWithinRange == false) 
    {
        MoveCharacter();

        TaskManager::Add()->Initialize(this, &ArtificialController::PerformAction, 0.1f);
    }
    else if(isWithinRange == true)
    {
        if(primaryTarget.IsTriggerValid())
        {
            battleController->TargetCombatant(primaryTarget.Target->Combatant);
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
            selectedCombatant->character->SelectAction(CharacterActions::DODGE);
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
    if(opportunityTarget.IsTriggerValid())
    {
        selectedCombatant->character->SelectAction(opportunityTarget.Action);

        if(opportunityTarget.Action == CharacterActions::ATTACK)
        {
            selectedCombatant->character->SelectWeapon(opportunityTarget.WeaponType);
        }
        else
        {
            selectedCombatant->character->SelectSpell(opportunityTarget.SpellType);
        }

        bool isWithinRange = selectedCombatant->character->GetActionRange() >= opportunityTarget.Target->Distance;  

        if(isWithinRange)
        {
            battleController->TargetCombatant(opportunityTarget.Target->Combatant);
            battleController->Act();

            return true;
        }
    }

    return false;
}

void ArtificialController::UpdateCharacter()
{
    battleController = BattleController::Get();
    battleScene = BattleScene::Get();
    selectedCombatant = battleController->GetSelectedCombatant();

    hasActed = false;

    DetermineActionCourse();

    PerformAction();
}
