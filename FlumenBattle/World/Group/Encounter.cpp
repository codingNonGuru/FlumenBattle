#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/Group/Group.h"

namespace world::group
{
    void Encounter::Initialize(Group * first_, Group * second_)
    {
        first = first_; 
        second = second_; 
        isOngoing = true;

        first->EngageGroup(this);
        second->EngageGroup(this);
    }

    void Encounter::Update()
    {

    }

    void Encounter::Finish() 
    {
        isOngoing = false;

        first->ExitBattle();
        second->ExitBattle();
    }
}