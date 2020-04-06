#include "Battle.hpp"

GroupBattleData::GroupBattleData(class Group * group) : Group(group)
{
    //TroopDatas.
}

Battle::Battle(Group * first_, Group * second_) : first(first_), second(second_)
{

}

void Battle::Update()
{

}