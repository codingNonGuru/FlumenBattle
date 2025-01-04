#include "NeighborHandler.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Polity/Neighbor.h"

using namespace world::polity;

void NeighborHandler::CalculateNeighbors(Polity *polity)
{
    neighbors.Reset();

    for(auto &settlement : polity->GetSettlements())
    {
        if(settlement->IsValid() == false)  
            continue;

        for(auto &link : settlement->GetLinks())
        {
            if(link.Other->IsValid() == false)
                continue;

            auto otherPolity = link.Other->GetPolity();
            if(otherPolity == polity)
                continue;

            auto neighbor = neighbors.Find(otherPolity);
            if(neighbor == nullptr)
            {
                *neighbors.Add() = {otherPolity, 1};
            }
            else
            {
                neighbor->LinkCount++;
            }
        }
    }
}

void NeighborAllocator::Allocate(container::ArrayAllocator <Neighbor> &allocator, NeighborHandler &handler) 
{
    handler.neighbors.Initialize(allocator);
}

void NeighborAllocator::Free(container::ArrayAllocator <Neighbor> &allocator, NeighborHandler &handler) 
{
    handler.neighbors.Terminate(allocator);
}