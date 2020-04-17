#include "FlumenCore/Utility/Utility.hpp"

#include "FlumenEngine/Core/TaskManager.hpp"

#include "FlumenBattle/ArtificialController.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleTile.h"

struct TileMoveData
{
    BattleTile *Tile;

    Integer Distance;

    bool operator < (const TileMoveData &other) {return this->Distance < other.Distance;}
};

Array <TileMoveData> tileMoveDatas = Array <TileMoveData> (64);

Character *attackTarget = nullptr;

Integer targetDistance = 0;

void ArtificialController::DetermineActionCourse()
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    selectedCharacter->SelectAction(CharacterActions::ATTACK);

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

    attackTarget = closestCharacter;
    targetDistance = closestDistance;
}

void ArtificialController::MoveCharacter()
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    if(attackTarget != nullptr)
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
            distance += tile->GetDistanceTo(*attackTarget->GetTile());

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

        targetDistance = selectedCharacter->GetDistanceTo(attackTarget);
    }
}

void ArtificialController::UpdateCharacter()
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    DetermineActionCourse();

    bool isWithinRange = selectedCharacter->GetActionRange() >= targetDistance;

    auto movement = selectedCharacter->GetMovement();
    if(movement > 0 && isWithinRange == false)
    {
        MoveCharacter();

        TaskManager::Add()->Initialize(this, &ArtificialController::UpdateCharacter, 0.1f);
    }
    else 
    {
        if(isWithinRange)
        {
            battleController->TargetCharacter(attackTarget);
            battleController->Act();
        }
        else
        {
            selectedCharacter->SelectAction(CharacterActions::DODGE);
            battleController->Act();
        }

        TaskManager::Add()->Initialize(battleController, &BattleController::EndTurn, 0.7f);
    }
}
