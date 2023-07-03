#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Group/MachineMind.h"

namespace world::group
{
    const GroupType * GroupTypeFactory::BuildPlayerGroupType()
    {
        static GroupType type = {GroupTypes::PLAYER, HumanMind::Get(), 5};
        return &type;
    }

    const GroupType * GroupTypeFactory::BuildComputerGroupType()
    {
        static GroupType type = {GroupTypes::COMPUTER, MachineMind::Get(), 5};
        return &type;
    }

    const GroupType * GroupTypeFactory::BuildGroupType(GroupTypes type)
    {
        switch(type)
        {
            case GroupTypes::PLAYER:
                return BuildPlayerGroupType();
            case GroupTypes::COMPUTER:
                return BuildComputerGroupType();
        }
    }
}