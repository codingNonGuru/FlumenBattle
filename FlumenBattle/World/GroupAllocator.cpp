#include "FlumenCore/Container/Array.hpp"

#include "FlumenBattle/World/GroupAllocator.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldScene.h"

namespace world
{
    void GroupAllocator::PerformCleanup()
    {
        auto scene = WorldScene::Get();

        static auto finishedGroups = Array <group::Group *> (scene->groups.GetCapacity());

        for(auto &group : scene->groups)
        {
            if(group.IsAlive())
                continue;
            
            *finishedGroups.Allocate() = &group;
        }

        for(auto group : finishedGroups)
        {
            scene->groups.Remove(group);
        }

        finishedGroups.Reset();
    }
}