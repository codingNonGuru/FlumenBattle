#include "FlumenBattle/Battle.h"
#include "FlumenBattle/World/Group/Group.h"

namespace world
{
    Battle::Battle(group::Group * first_, group::Group * second_) : first(first_), second(second_), isOngoing(true)
    {
        first->EnterBattle(this);
        second->EnterBattle(this);
    }

    void Battle::Update()
    {

    }

    void Battle::Finish() 
    {
        isOngoing = false;

        first->ExitBattle();
        second->ExitBattle();
    }
}