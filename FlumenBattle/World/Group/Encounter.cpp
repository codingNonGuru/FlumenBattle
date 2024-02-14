#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/Group/Group.h"

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
    }

    void Encounter::Update()
    {

    }

    void Encounter::Finish(Group *winner) 
    {
        isOngoing = false;

        this->winner = winner;

        //first->ExitBattle();
        //second->ExitBattle();
    }
}