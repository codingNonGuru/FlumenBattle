#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldScene.h"

namespace world::group
{
    void Encounter::Initialize(Group * first_, Group * second_)
    {
        first = first_; 
        second = second_;

        isOngoing = true;

        isBattle = false;

        hasBattleEnded = false;

        first->EngageGroup(this);
        second->EngageGroup(this);
        
        winner = nullptr;

        static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        isPlayerInvolved = playerGroup == first || playerGroup == second;
    }

    void Encounter::Update()
    {
        if(isPlayerInvolved == true)
            return;
    }

    void Encounter::Finish(Group *winner) 
    {
        isOngoing = false;

        this->winner = winner;

        //first->ExitBattle();
        //second->ExitBattle();
    }
}