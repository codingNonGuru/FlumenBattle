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

struct CombatantData
{
    class Combatant *Combatant;

    Integer Distance;
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

CombatantData ArtificialController::FindCombatant(FilterList filterData, CombatantSinglingCriteria singlingCriterion)
{
    static Array <CombatSearchFilter> filters = Array <CombatSearchFilter> (16);
    filters.Reset();

    for(auto filter : filterData)
    {
        *filters.Allocate() = filter;
    }

    static Array <CombatGroup *> groups = Array <CombatGroup *> (2);
    groups.Reset();

    if(filters.Find(CombatSearchFilterTypes::IS_ALLY))
    {
        *groups.Allocate() = battleScene->GetComputerGroup();
    }

    if(filters.Find(CombatSearchFilterTypes::IS_ENEMY))
    {
        *groups.Allocate() = battleScene->GetPlayerGroup();
    }

    Combatant *searchResult = nullptr;

    for(auto group = groups.GetStart(); group != groups.GetEnd(); ++group)
    {
        auto &combatants = (*group)->GetCombatants();
        for(auto combatant = combatants.GetStart(); combatant != combatants.GetEnd(); ++combatant)
        {
            if(filters.Find(CombatSearchFilterTypes::IS_ALIVE))
            {
                if(combatant->IsAlive() == false)
                    continue;
            }

            if(auto filter = filters.Find(CombatSearchFilterTypes::IS_WITHIN_RANGE))
            {
                auto distance = selectedCombatant->GetDistanceTo(combatant);
                if(distance > filter->Value)
                    continue;
            }

            if(filters.Find(CombatSearchFilterTypes::IS_VULNERABLE))
            {   
                if(combatant->GetHealth() > 0.5f && combatant->armorClass > 12)
                    continue;
            }

            if(searchResult != nullptr)
            {
                if(singlingCriterion == CombatantSinglingCriteria::IS_CLOSEST)
                {
                    auto distance = selectedCombatant->GetDistanceTo(combatant);
                    if(distance < selectedCombatant->GetDistanceTo(searchResult))
                    {
                        searchResult = combatant;
                    }
                }
                else if(singlingCriterion == CombatantSinglingCriteria::IS_MOST_VULNERABLE)
                {
                    if(combatant->GetHealth() < searchResult->GetHealth())
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

    auto distance = searchResult != nullptr ? selectedCombatant->GetDistanceTo(searchResult) : 0;
    return {searchResult, distance};
}

void ArtificialController::DetermineActionCourse()
{
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
            /*if(allyData.Combatant != nullptr)
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
            }*/
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

    auto combatantData = FindCombatant(
        {CombatSearchFilterTypes::IS_ALIVE, CombatSearchFilterTypes::IS_ENEMY, {CombatSearchFilterTypes::IS_WITHIN_RANGE, 1}}, 
        CombatantSinglingCriteria::IS_MOST_VULNERABLE);

    if(combatantData.Combatant != nullptr)
    {
        *actionQueue.Allocate() = {combatantData.Combatant, WeaponTypes::GREAT_SWORD};
    }
    else
    {
        auto combatantData = FindCombatant(
            {CombatSearchFilterTypes::IS_ALIVE, CombatSearchFilterTypes::IS_ENEMY}, 
            CombatantSinglingCriteria::IS_CLOSEST);

        if(combatantData.Combatant != nullptr)
        {
            auto movement = selectedCombatant->GetMovement();
            bool hasReached = false; 

            auto startTile = selectedCombatant->GetTile();
            auto endTile = combatantData.Combatant->GetTile();
            while(true)
            {
                startTile = FindClosestFreeTile(startTile, endTile);

                *actionQueue.Allocate() = {startTile};

                movement--;
                
                hasReached = startTile->GetDistanceTo(*endTile) == 1;

                if(movement == 0 || hasReached)
                {
                    break;
                }
            }

            if(hasReached)
            {
                *actionQueue.Allocate() = {combatantData.Combatant, WeaponTypes::GREAT_SWORD};
            }
            else
            {
                *actionQueue.Allocate() = {CharacterActions::DODGE};
            }
        }
    }
}

void ArtificialController::DetermineRangerBehavior()
{
    selectedCombatant->character->SelectAction(CharacterActions::ATTACK);

    auto combatantData = FindCombatant(
        {CombatSearchFilterTypes::IS_ALIVE, CombatSearchFilterTypes::IS_ENEMY, {CombatSearchFilterTypes::IS_WITHIN_RANGE, 1}}, 
        CombatantSinglingCriteria::IS_MOST_VULNERABLE);

    if(combatantData.Combatant != nullptr)
    {
        *actionQueue.Allocate() = {combatantData.Combatant, WeaponTypes::SHORT_SWORD};
    }
    else
    {
        selectedCombatant->character->SelectWeapon(WeaponTypes::LONG_BOW);
        auto range = selectedCombatant->character->GetActionRange();

        auto mostVulnerableEnemy = FindCombatant(
            {CombatSearchFilterTypes::IS_ALIVE, CombatSearchFilterTypes::IS_ENEMY, {CombatSearchFilterTypes::IS_WITHIN_RANGE, range}}, 
            CombatantSinglingCriteria::IS_MOST_VULNERABLE);

        auto closestVulnerableEnemy = FindCombatant(
            {CombatSearchFilterTypes::IS_ALIVE, CombatSearchFilterTypes::IS_ENEMY, CombatSearchFilterTypes::IS_VULNERABLE}, 
            CombatantSinglingCriteria::IS_CLOSEST);

        auto closestEnemy = FindCombatant(
            {CombatSearchFilterTypes::IS_ALIVE, CombatSearchFilterTypes::IS_ENEMY}, 
            CombatantSinglingCriteria::IS_CLOSEST);

        CombatantData target;
        if(mostVulnerableEnemy.Combatant != nullptr)
        {
            target = mostVulnerableEnemy;
        }
        else if(closestVulnerableEnemy.Combatant != nullptr)
        {
            target = closestVulnerableEnemy;
        }
        else
        {
            target = closestEnemy;
        }

        if(target.Combatant != nullptr && target.Distance > range)
        {
            auto movement = selectedCombatant->GetMovement();
            bool hasReached = false; 

            auto startTile = selectedCombatant->GetTile();
            auto endTile = target.Combatant->GetTile();
            while(true)
            {
                startTile = FindClosestFreeTile(startTile, endTile);

                *actionQueue.Allocate() = {startTile};

                movement--;
                
                hasReached = startTile->GetDistanceTo(*endTile) == range;

                if(movement == 0 || hasReached)
                {
                    break;
                }
            }

            if(hasReached)
            {
                *actionQueue.Allocate() = {target.Combatant, WeaponTypes::LONG_BOW};
            }
            else
            {
                *actionQueue.Allocate() = {CharacterActions::DODGE};
            }
        }
        else if(target.Distance <= range)
        {
            *actionQueue.Allocate() = {target.Combatant, WeaponTypes::LONG_BOW};
        }
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
    else if(action->Action == CharacterActions::ATTACK)
    {
        selectedCombatant->character->SelectAction(action->Action);
        selectedCombatant->character->SelectWeapon(action->WeaponType);

        battleController->TargetCombatant(action->Target);
        battleController->Act();
    }
    else if(action->Action == CharacterActions::CAST_SPELL)
    {
        selectedCombatant->character->SelectAction(action->Action);
        selectedCombatant->character->SelectSpell(action->SpellType);

        battleController->TargetCombatant(action->Target);
        battleController->Act();
    }
    else
    {
        selectedCombatant->character->SelectAction(action->Action); 

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
