#pragma once

#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/Race.h"

namespace world::character
{
    struct RecruitData
    {
        unsigned int UniqueId;

        CharacterClasses Class;

        const Race *Race;

        int Level;

        int Cost;

        int IconTextureId;

        bool operator==(unsigned int uniqueId) {return this->UniqueId == uniqueId;}
    };
}