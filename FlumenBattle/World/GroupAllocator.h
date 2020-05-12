#pragma once

#include "FlumenEngine/Core/Singleton.h"

namespace world
{
    class GroupAllocator : public Singleton<GroupAllocator>
    {
        friend class WorldScene;

        void PerformCleanup();
    };
}