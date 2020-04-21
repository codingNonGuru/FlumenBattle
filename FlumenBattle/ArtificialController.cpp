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
#include "FlumenBattle/CharacterClass.h"

enum class NarrowingCriteria
{
    IS_NOT_SELF, IS_ALLY, IS_ENEMY, IS_VULNERABLE, IS_WITHIN_RANGE, IS_ALIVE, IS_CLASS, IS_NOT_CLASS
};

enum class SinglingCriteria
{
    IS_CLOSEST, IS_LEAST_HEALTHY, IS_LEAST_ARMORED
};

struct CombatSearchFilter
{
    NarrowingCriteria Type;

    Integer Value;

    bool operator== (const CombatSearchFilter &filter) {return Type == filter.Type;}

    CombatSearchFilter() {}

    CombatSearchFilter(NarrowingCriteria type) : Type(type) {}

    CombatSearchFilter(NarrowingCriteria type, Integer value) : Type(type), Value(value) {}

    CombatSearchFilter(NarrowingCriteria type, CharacterClasses value) : Type(type), Value((Integer)value) {}
};

struct CombatantData
{
    class Combatant *Combatant;

    Integer Distance;

    operator bool() {return Combatant != nullptr;}

    operator class Combatant *() {return Combatant;}
};

struct ActionData
{
    Combatant *Target;

    BattleTile *Destination;

    CharacterActions Action;

    WeaponTypes WeaponType;

    SpellTypes SpellType;

    ActionData() {}

    ActionData(CharacterActions action) : Action(action) {}

    ActionData(BattleTile *destination) : Action(CharacterActions::MOVE), Destination(destination) {}

    ActionData(Combatant *target, WeaponTypes weaponType) :
        Target(target), WeaponType(weaponType), Action(CharacterActions::ATTACK) {}

    ActionData(Combatant *target, SpellTypes spellType) :
        Target(target), SpellType(spellType), Action(CharacterActions::CAST_SPELL) {}
};

struct TileMoveData
{
    BattleTile *Tile;

    Integer Distance;

    bool operator < (const TileMoveData &other) {return this->Distance < other.Distance;}
};

Array <TileMoveData> tileMoveDatas = Array <TileMoveData> (64);

BattleController *battleController = nullptr;

BattleScene *battleScene = nullptr;

Combatant *selectedCombatant = nullptr;

static Array <ActionData> actionQueue = Array <ActionData> (32);

static Integer currentActionIndex = 0;

BattleTile * ArtificialController::FindClosestFreeTile(BattleTile *source, BattleTile *destination)
{
    auto &nearbyTiles = source->GetNearbyTiles(1);
    for(auto tileIterator = nearbyTiles.GetStart(); tileIterator != nearbyTiles.GetEnd(); ++tileIterator)
    {
        auto tile = *tileIterator;
        if(tile->Combatant != nullptr)
            continue;

        bool hasVisited = false;
        for(auto action = actionQueue.GetStart(); action != actionQueue.GetEnd(); ++action)
        {
            if(action->Action == CharacterActions::MOVE && action->Destination == tile)
            {
                hasVisited = true;
                break;
            }
        }

        if(hasVisited)
            continue;

        Integer distance = tile->GetDistanceTo(*destination);

        *tileMoveDatas.Allocate() = {tile, distance};
    }

    tileMoveDatas.SortAscendantly();

    Integer closestDistance = tileMoveDatas.GetStart()->Distance;
    Index dataIndex = 0;
    for(auto data = tileMoveDatas.GetStart(); data != tileMoveDatas.GetEnd(); ++data, ++dataIndex)
    {
        if(data->Distance > closestDistance)
        {
            break;
        }
    }

    dataIndex = utility::GetRandom(0, dataIndex - 1);
    BattleTile *targetedTile = tileMoveDatas.Get(dataIndex)->Tile;

    tileMoveDatas.Reset();

    return targetedTile;
}

CombatantData ArtificialController::FindCombatant(FilterList filterData, SinglingCriteria singlingCriterion)
{
    auto virtualPosition = GetVirtualTile();

    static Array <CombatSearchFilter> filters = Array <CombatSearchFilter> (16);

    filters.Reset();
    for(auto filter : filterData)
    {
        *filters.Allocate() = filter;
    }

    static Array <CombatGroup *> groups = Array <CombatGroup *> (2);
    groups.Reset();

    if(filters.Find(NarrowingCriteria::IS_ALLY))
    {
        *groups.Allocate() = battleScene->GetComputerGroup();
    }

    if(filters.Find(NarrowingCriteria::IS_ENEMY))
    {
        *groups.Allocate() = battleScene->GetPlayerGroup();
    }

    Combatant *searchResult = nullptr;

    for(auto group = groups.GetStart(); group != groups.GetEnd(); ++group)
    {
        auto &combatants = (*group)->GetCombatants();
        for(auto combatant = combatants.GetStart(); combatant != combatants.GetEnd(); ++combatant)
        {
            if(filters.Find(NarrowingCriteria::IS_NOT_SELF))
            {
                if(combatant == selectedCombatant)
                    continue;
            }
            
            if(filters.Find(NarrowingCriteria::IS_ALIVE))
            {
                if(combatant->IsAlive() == false)
                    continue;
            }

            if(auto filter = filters.Find(NarrowingCriteria::IS_WITHIN_RANGE))
            {
                auto distance = virtualPosition->GetDistanceTo(*combatant->GetTile());
                if(distance > filter->Value)
                    continue;
            }

            if(filters.Find(NarrowingCriteria::IS_VULNERABLE))
            {   
                if(combatant->GetHealth() > 0.5f)
                    continue;
            }

            if(auto filter = filters.Find(NarrowingCriteria::IS_CLASS))
            {   
                if(combatant->character->type->Class != (CharacterClasses)filter->Value)
                    continue;
            }

            if(auto filter = filters.Find(NarrowingCriteria::IS_NOT_CLASS))
            {   
                if(combatant->character->type->Class == (CharacterClasses)filter->Value)
                    continue;
            }

            if(searchResult != nullptr)
            {
                if(singlingCriterion == SinglingCriteria::IS_CLOSEST)
                {
                    auto distance = virtualPosition->GetDistanceTo(*combatant->GetTile());
                    if(distance < virtualPosition->GetDistanceTo(*searchResult->GetTile()))
                    {
                        searchResult = combatant;
                    }
                }
                else if(singlingCriterion == SinglingCriteria::IS_LEAST_HEALTHY)
                {
                    if(combatant->character->currentHitPoints < searchResult->character->currentHitPoints)
                    {
                        searchResult = combatant;
                    }
                }
                else if(singlingCriterion == SinglingCriteria::IS_LEAST_ARMORED)
                {
                    if(combatant->armorClass < searchResult->armorClass)
                    {
                        searchResult = combatant;
                    }
                }
            }
            else
            {
                searchResult = combatant;
            }
        }
    }

    auto distance = searchResult != nullptr ? virtualPosition->GetDistanceTo(*searchResult->GetTile()) : 0;
    return {searchResult, distance};
}

Integer virtualMovement = 0;

bool ArtificialController::ApproachTile(BattleTile *destination, Integer range)
{
    if(virtualMovement == 0)
    {
        return false;
    }

    bool hasReached = false; 

    auto startTile = GetVirtualTile();

    while(true)
    {
        startTile = FindClosestFreeTile(startTile, destination);

        *actionQueue.Allocate() = {startTile};

        virtualMovement--;
        
        hasReached = startTile->GetDistanceTo(*destination) == range;

        if(virtualMovement == 0 || hasReached)
        {
            break;
        }
    }

    return hasReached;
}

BattleTile * ArtificialController::GetVirtualTile()
{
    ActionData *lastAction = nullptr;
    for(auto action = actionQueue.GetStart(); action != actionQueue.GetEnd(); ++action)
    {
        if(action->Action == CharacterActions::MOVE)
        {
            lastAction = action;
        }
    }

    return lastAction ? lastAction->Destination : selectedCombatant->GetTile();
}

void ArtificialController::DetermineActionCourse()
{
    virtualMovement = selectedCombatant->GetMovement();

    actionQueue.Reset();

    switch(selectedCombatant->character->type->Class)
    {
        case CharacterClasses::FIGHTER:
            DetermineFighterBehavior();
            break;
        case CharacterClasses::RANGER:
            DetermineRangerBehavior();
            break;
        case CharacterClasses::CLERIC:
            DetermineClericBehavior();
            break;
        case CharacterClasses::WIZARD:
            /*selectedCombatant->character->SelectAction(CharacterActions::CAST_SPELL);
            selectedCombatant->character->SelectSpell(SpellTypes::FROST_RAY);

            primaryTarget = {&enemyData, ActionTriggers::NONE, SpellTypes::FROST_RAY};
            opportunityTarget = primaryTarget;*/
            break;
    }
}

void ArtificialController::DetermineFighterBehavior()
{
    selectedCombatant->character->SelectAction(CharacterActions::ATTACK);

    selectedCombatant->character->SelectWeapon(WeaponTypes::GREAT_SWORD);

    auto nearbyEnemy = FindCombatant(
        {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ENEMY, {NarrowingCriteria::IS_WITHIN_RANGE, 1}}, 
        SinglingCriteria::IS_LEAST_HEALTHY);

    if(nearbyEnemy != nullptr)
    {
        *actionQueue.Allocate() = {nearbyEnemy, WeaponTypes::GREAT_SWORD};
    }
    else
    {
        auto closestEnemy = FindCombatant(
            {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ENEMY}, 
            SinglingCriteria::IS_CLOSEST);

        if(closestEnemy != nullptr)
        {
            bool hasReached = ApproachTile(closestEnemy.Combatant->GetTile(), 1);

            if(hasReached)
            {
                *actionQueue.Allocate() = {closestEnemy, WeaponTypes::GREAT_SWORD};
            }
            else
            {
                *actionQueue.Allocate() = {CharacterActions::DASH};
                virtualMovement += selectedCombatant->GetMovement();

                ApproachTile(closestEnemy.Combatant->GetTile(), 1);
            }
        }
    }
}

void ArtificialController::DetermineRangerBehavior()
{
    selectedCombatant->character->SelectAction(CharacterActions::ATTACK);

    auto nearbyEnemy = FindCombatant(
        {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ENEMY, {NarrowingCriteria::IS_WITHIN_RANGE, 1}}, 
        SinglingCriteria::IS_LEAST_HEALTHY);

    if(nearbyEnemy)
    {
        *actionQueue.Allocate() = {nearbyEnemy, WeaponTypes::SHORT_SWORD};
    }
    else
    {
        selectedCombatant->character->SelectWeapon(WeaponTypes::LONG_BOW);
        auto range = selectedCombatant->character->GetActionRange();

        auto mostVulnerableEnemy = FindCombatant(
            {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ENEMY, NarrowingCriteria::IS_VULNERABLE, {NarrowingCriteria::IS_WITHIN_RANGE, range}}, 
            SinglingCriteria::IS_LEAST_ARMORED);

        auto closeVulnerableEnemy = FindCombatant(
            {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ENEMY, {NarrowingCriteria::IS_WITHIN_RANGE, range}}, 
            SinglingCriteria::IS_LEAST_ARMORED);

        auto closestEnemy = FindCombatant(
            {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ENEMY}, 
            SinglingCriteria::IS_CLOSEST);

        CombatantData target;
        if(mostVulnerableEnemy)
        {
            target = mostVulnerableEnemy;
        }
        else if(closeVulnerableEnemy)
        {
            target = closeVulnerableEnemy;
        }
        else
        {
            target = closestEnemy;
        }

        if(target && target.Distance > range)
        {
            bool hasReached = ApproachTile(target.Combatant->GetTile(), range);

            if(hasReached)
            {
                *actionQueue.Allocate() = {target, WeaponTypes::LONG_BOW};
            }
            else
            {
                *actionQueue.Allocate() = {CharacterActions::DODGE};
            }
        }
        else if(target.Distance <= range)
        {
            *actionQueue.Allocate() = {target, WeaponTypes::LONG_BOW};
        }
    }
}

void ArtificialController::DetermineClericBehavior()
{
    auto nearbyEndangeredAlly = FindCombatant(
        {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ALLY, NarrowingCriteria::IS_VULNERABLE, {NarrowingCriteria::IS_WITHIN_RANGE, 1}}, 
        SinglingCriteria::IS_LEAST_HEALTHY);

    auto nearbyDangerousEnemy = FindCombatant(
        {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ENEMY, {NarrowingCriteria::IS_WITHIN_RANGE, 1}}, 
        SinglingCriteria::IS_LEAST_ARMORED);

    bool hasAction = false;

    if(nearbyEndangeredAlly && nearbyDangerousEnemy)
    {
        *actionQueue.Allocate() = {nearbyEndangeredAlly, SpellTypes::CURE_WOUNDS};
        hasAction = true;
    }
    else if(nearbyDangerousEnemy)
    {
        *actionQueue.Allocate() = {nearbyDangerousEnemy, WeaponTypes::MACE};
        hasAction = true;
    }

    if(hasAction == false)
    {
        auto mostVulnerableAlly = FindCombatant(
            {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ALLY, NarrowingCriteria::IS_VULNERABLE, {NarrowingCriteria::IS_WITHIN_RANGE, virtualMovement}}, 
            SinglingCriteria::IS_LEAST_HEALTHY);

        if(mostVulnerableAlly)
        {
            if(mostVulnerableAlly != selectedCombatant)
            {
                bool hasReached = ApproachTile(mostVulnerableAlly.Combatant->GetTile(), 1);

                if(hasReached)
                {
                    *actionQueue.Allocate() = {mostVulnerableAlly, SpellTypes::CURE_WOUNDS};
                    hasAction = true;
                }
            }
            else
            {
                *actionQueue.Allocate() = {mostVulnerableAlly, SpellTypes::CURE_WOUNDS};
                hasAction = true;
            }
        }
    }

    auto FindSacredFlameTarget = [this, &hasAction]
    {
        auto range = SpellFactory::BuildSacredFlame().Range;

        auto mostVulnerableEnemy = FindCombatant(
            {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ENEMY, NarrowingCriteria::IS_VULNERABLE, {NarrowingCriteria::IS_WITHIN_RANGE, range}}, 
            SinglingCriteria::IS_LEAST_HEALTHY);

        auto closeVulnerableEnemy = FindCombatant(
            {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ENEMY, {NarrowingCriteria::IS_WITHIN_RANGE, range}}, 
            SinglingCriteria::IS_LEAST_HEALTHY);

        if(mostVulnerableEnemy)
        {
            *actionQueue.Allocate() = {mostVulnerableEnemy, SpellTypes::SACRED_FLAME};
        }
        else if(closeVulnerableEnemy)
        {
            *actionQueue.Allocate() = {closeVulnerableEnemy, SpellTypes::SACRED_FLAME};
        }

        hasAction = true;
    };

    if(hasAction == false)
    {
        FindSacredFlameTarget();
    }

    auto escortableAlly = FindCombatant(
        {NarrowingCriteria::IS_ALIVE, NarrowingCriteria::IS_ALLY, {NarrowingCriteria::IS_CLASS, CharacterClasses::FIGHTER}}, 
        SinglingCriteria::IS_CLOSEST);

    if(escortableAlly)
    {
        ApproachTile(escortableAlly.Combatant->GetTile(), 1);
    }

    if(hasAction == false)
    {
        FindSacredFlameTarget();
    }

    if(hasAction == false)
    {
        *actionQueue.Allocate() = {CharacterActions::DODGE};
    }
}

void ArtificialController::MoveCharacter()
{
    auto action = actionQueue.Get(currentActionIndex);

    if(action->Destination != nullptr)
    {
        battleController->TargetTile(action->Destination);
        battleController->Move();
    }
}

void ArtificialController::PerformAction()
{
    if(actionQueue.IsEmpty())
    {
        TaskManager::Add()->Initialize(battleController, &BattleController::EndTurn, 0.7f);
        return;
    }

    auto action = actionQueue.Get(currentActionIndex);

    if(action->Action == CharacterActions::MOVE)
    {
        MoveCharacter();
    }
    else
    {
        selectedCombatant->character->SelectAction(action->Action);

        if(action->Action == CharacterActions::ATTACK)
        {
            selectedCombatant->character->SelectWeapon(action->WeaponType);

            battleController->TargetCombatant(action->Target);
        }
        else if(action->Action == CharacterActions::CAST_SPELL)
        {
            selectedCombatant->character->SelectSpell(action->SpellType);

            battleController->TargetCombatant(action->Target);
        }

        battleController->Act();
    }

    currentActionIndex++;

    if(currentActionIndex == actionQueue.GetSize())
    {
        TaskManager::Add()->Initialize(battleController, &BattleController::EndTurn, 0.7f);
    }
    else
    {
        TaskManager::Add()->Initialize(this, &ArtificialController::PerformAction, 0.1f);
    }
}

void ArtificialController::UpdateCharacter()
{
    battleController = BattleController::Get();
    battleScene = BattleScene::Get();
    selectedCombatant = battleController->GetSelectedCombatant();

    currentActionIndex = 0;

    DetermineActionCourse();

    PerformAction();
}
