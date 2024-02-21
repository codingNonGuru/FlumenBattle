#pragma once

#include "FlumenBattle/World/Character/Types.h"

namespace world::group
{
    struct ItemChange
    {
        character::ItemTypes Type;

        int Amount;
    };
}