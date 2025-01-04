#pragma once

#include <mutex>

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/Array.hpp"
#include "FlumenCore/Container/Map.hpp"
#include "FlumenCore/Singleton.h"

#include "FlumenEngine/Thread/ThreadManager.h"

#include "FlumenBattle/World/Tile/TileBuffer.h"

namespace world::tile
{
    template <typename TileType>
    class TileBufferManager : public core::Singleton <TileBufferManager <TileType>>
    {
        friend class core::Singleton <TileBufferManager>;

        TileBufferBatch <TileType> bufferBatches[engine::ThreadManager::GetThreadCount() + 2];

        container::StaticMap <TileBufferBatch <TileType> *, std::thread::id> bufferBatchMap {engine::ThreadManager::GetThreadCount() + 2};

        int lastBatchIndex {0};

    public:
        TileBuffer <TileType> *GetUsableBuffer()
        {
            const auto threadId = std::this_thread::get_id();

            auto batchPointer = bufferBatchMap.Get(threadId);
            if(batchPointer != nullptr)
            {
                auto batch = *batchPointer;

                auto buffer = batch->Buffers.Add();
                buffer->Tiles.Reset();

                return buffer;
            }

            std::mutex mutex;
            mutex.lock();

            batchPointer = bufferBatchMap.Add(threadId);

            *batchPointer = &bufferBatches[lastBatchIndex];
            lastBatchIndex++;

            mutex.unlock();

            auto batch = *batchPointer;
                
            auto buffer = batch->Buffers.Add();
            buffer->Tiles.Reset();

            return buffer;
        }
    };
}