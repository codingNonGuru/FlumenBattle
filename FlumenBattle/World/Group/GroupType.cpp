#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Group/MachineMind.h"

namespace world::group
{
    const GroupType * GroupTypeFactory::BuildPlayerGroupType()
    {
        static GroupType type = {GroupTypes::PLAYER, GroupClasses::PLAYER, HumanMind::Get(), 5};
        return &type;
    }

    const GroupType * GroupTypeFactory::BuildComputerGroupType(GroupClasses groupClass)
    {
        switch(groupClass)
        {
        case GroupClasses::ADVENTURER:
            {
                static const GroupType type = {GroupTypes::COMPUTER, groupClass, MachineMind::Get(), 1};
                return &type;
            }
        case GroupClasses::MERCHANT:
            {
                static const GroupType type = {GroupTypes::COMPUTER, groupClass, MachineMind::Get(), 1};
                return &type;
            }
        case GroupClasses::BANDIT:
            {
                static const GroupType type = {GroupTypes::COMPUTER, groupClass, MachineMind::Get(), 1};
                return &type;
            }
        case GroupClasses::PATROL:
            {
                static const GroupType type = {GroupTypes::COMPUTER, groupClass, MachineMind::Get(), 1};
                return &type;
            }
        }
    }

    const GroupType * GroupTypeFactory::BuildGroupType(GroupClasses groupClass)
    {
        switch(groupClass)
        {
            case GroupClasses::PLAYER:
                return BuildPlayerGroupType();
            default:
                return BuildComputerGroupType(groupClass);
        }
    }
}