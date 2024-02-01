#pragma once

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/Array.hpp"
#include "FlumenCore/Container/Map.hpp"
#include "FlumenCore/Singleton.h"

#include "FlumenEngine/Thread/ThreadManager.h"

namespace world
{
    template <typename ResourceType>
    class ThreadedResourceHandler : public core::Singleton <ThreadedResourceHandler <ResourceType>>
    {
        friend class core::Singleton <ThreadedResourceHandler>;

        ResourceType resources[engine::ThreadManager::GetThreadCount() + 2];

        container::StaticMap <ResourceType *, std::thread::id> resourceMap {engine::ThreadManager::GetThreadCount() + 2};

        int lastBatchIndex {0};

    public:
        ResourceType *GetUsableBuffer()
        {
            const auto threadId = std::this_thread::get_id();

            auto resourcePointer = resourceMap.Get(threadId);
            if(resourcePointer != nullptr)
            {
                return *resourcePointer;
            }

            resourcePointer = resourceMap.Add(threadId);

            *resourcePointer = &resources[lastBatchIndex];
            lastBatchIndex++;

            return *resourcePointer;
        }
    };
}