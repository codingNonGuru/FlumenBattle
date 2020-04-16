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

void ArtificialController::UpdateCharacter()
{
    auto battleController = BattleController::Get();
    auto selectedCharacter = battleController->selectedCharacter;

    Character *closestCharacter = nullptr;
    Integer closestDistance = INT_MAX;

    auto &characters = battleController->playerControlledGroup->GetCharacters();
    for(auto character = characters.GetStart(); character != characters.GetEnd(); ++character)
    {
        auto distance = selectedCharacter->GetDistanceTo(character);
        if(distance < closestDistance)
        {
            closestCharacter = character;
            closestDistance = distance;
        }
    }

    if(closestCharacter != nullptr)
    {
        BattleTile *targetTile = nullptr;
        closestDistance = INT_MAX;

        auto &nearbyTiles = selectedCharacter->GetTile()->GetNearbyTiles(1);
        for(auto tileIterator = nearbyTiles.GetStart(); tileIterator != nearbyTiles.GetEnd(); ++tileIterator)
        {
            auto tile = *tileIterator;
            if(tile->Character != nullptr)
                continue;

            Integer distance = selectedCharacter->GetTile()->GetDistanceTo(*tile);
            distance += tile->GetDistanceTo(*closestCharacter->GetTile());

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
    }

    auto movement = selectedCharacter->GetMovement();
    if(movement > 0)
    {
        TaskManager::Add()->Initialize(this, &ArtificialController::UpdateCharacter, 0.5f);
    }
    else
    {
        TaskManager::Add()->Initialize(battleController, &BattleController::EndTurn, 1.0f);
    }
}
