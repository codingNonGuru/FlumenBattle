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

Character *targetCharacter = nullptr;

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

    targetCharacter = closestCharacter;
    targetDistance = closestDistance;
}

void ArtificialController::MoveCharacter()
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    if(targetCharacter != nullptr)
    {
        BattleTile *targetTile = nullptr;
        Integer closestDistance = INT_MAX;

        auto &nearbyTiles = selectedCharacter->GetTile()->GetNearbyTiles(1);
        for(auto tileIterator = nearbyTiles.GetStart(); tileIterator != nearbyTiles.GetEnd(); ++tileIterator)
        {
            auto tile = *tileIterator;
            if(tile->Character != nullptr)
                continue;

            Integer distance = selectedCharacter->GetTile()->GetDistanceTo(*tile);
            distance += tile->GetDistanceTo(*targetCharacter->GetTile());

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
        targetTile = tileMoveDatas.Get(dataIndex)->Tile;

        bool hasMoved = selectedCharacter->Move(targetTile);

        tileMoveDatas.Reset();

        targetDistance = selectedCharacter->GetDistanceTo(targetCharacter);
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

        TaskManager::Add()->Initialize(this, &ArtificialController::UpdateCharacter, 0.3f);
    }
    else 
    {
        if(isWithinRange)
        {
            if(selectedCharacter->CanAct(targetCharacter))
            {
                //std::cout<<"ACT\n";
                selectedCharacter->Act(targetCharacter);
            }
        }
        else
        {
            selectedCharacter->SelectAction(CharacterActions::DODGE);
            if(selectedCharacter->CanAct(targetCharacter))
            {
                //std::cout<<"DODGE\n";
                selectedCharacter->Act(targetCharacter);
            }
        }

        TaskManager::Add()->Initialize(battleController, &BattleController::EndTurn, 1.0f);
    }
}
