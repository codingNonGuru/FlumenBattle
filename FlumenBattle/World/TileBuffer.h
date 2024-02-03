#pragma once

#include "FlumenCore/Container/Pool.hpp"
#include "FlumenCore/Container/Array.hpp"

namespace world
{
    #define TILES_PER_BUFFER 1024

    #define TILE_BUFFERS_PER_THREAD 8

    template <typename TileType>
    struct TileBuffer;

    template <typename TileType>
    struct TileBufferBatch
    {
        container::Pool <TileBuffer <TileType>> Buffers;

        TileBufferBatch() : Buffers(TILE_BUFFERS_PER_THREAD)
        {
            for(int i = 0; i < Buffers.GetCapacity(); ++i)
            {
                auto buffer = Buffers.Add();

                buffer->Initialize(this);
            }

            Buffers.Reset();
        }
    };

    template <typename TileType>        
    struct TileBuffer
    {
        container::Array <TileType *> Tiles;

        TileBufferBatch <TileType> *Batch {nullptr};

        TileBuffer() : Tiles(TILES_PER_BUFFER) {}

        void Initialize(TileBufferBatch <TileType> *batch) {Batch = batch;}

        ~TileBuffer()
        {
            //std::cout<<this<<" "<<Batch<<" "<< Tiles.GetStart() <<" "<<Batch->Buffers.GetSize()<<"\n";
            Batch->Buffers.Remove(Tiles.GetStart());
            //std::cout<<this<<" "<<Batch<<" "<< Tiles.GetStart() <<" "<<Batch->Buffers.GetSize()<<"\n\n";
        }

        TileBuffer(const TileBuffer &) = delete;

        TileBuffer& operator=(TileBuffer &buffer) = delete;

        TileBuffer(TileBuffer &&buffer) {Tiles = buffer.Tiles; Batch = buffer.Batch;}

        TileBuffer& operator=(TileBuffer &&buffer) {Tiles = buffer.Tiles; Batch = buffer.Batch; return *this;}

        bool operator== (TileType **tileStart) {return Tiles.GetStart() == tileStart;}

        int GetSize() const {return Tiles.GetSize();}

        auto begin() {return typename container::Array <TileType *>::Iterator <TileType *>(Tiles, Tiles.GetStart());}

        auto end() {return typename container::Array <TileType *>::Iterator <TileType *>(Tiles, Tiles.GetEnd());}
    };
}