#include "SettlementPathfinder.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Path.h"

using namespace utility;

SettlementPathfinder::SettlementPathData SettlementPathfinder::FindPathToSettlement(
    world::settlement::Settlement *start, 
    world::settlement::Settlement *end
)
{
    if(start == end)
    {
        visitedSettlements.Reset();
        *visitedSettlements.Add() = start;

        return {visitedSettlements, 0, visitedSettlements.GetSize()};
    }

    auto startClock = high_resolution_clock::now();

    settlementPaths.Clear();

    auto &settlements = world::WorldScene::Get()->GetSettlements();
    for(auto &settlement : settlements)
    {
        settlement.GetPathData().IsVisited = false;
        settlement.GetPathData().IsToBeVisited = true;
    }

    start->GetPathData().IsVisited = true;
    start->GetPathData().IsToBeVisited = true;

    visitedSettlements.Reset();
    *visitedSettlements.Add() = start;

    typename SettlementGraph::Node *championPath = settlementPaths.StartGraph({start, 0});

    int searches = 0;
    while(true)
    {
        auto bestComplexity = INT_MAX;
        typename SettlementGraph::Node *bestNode = nullptr;
        world::settlement::Settlement *bestSettlement = nullptr;
        
        for(auto &settlement : visitedSettlements)
        {
            searches++;
            if(settlement.Settlement->GetPathData().IsVisited == true)
            {
                auto &links = settlement.Settlement->GetLinks();
                for(auto &link : links)
                {
                    auto other = link.Path->GetOther(settlement.Settlement);
                    if(other->GetPathData().IsVisited == false && other->GetPathData().IsToBeVisited == true)
                    {
                        auto &nodes = settlementPaths.GetNodes();
                        for(auto &node : nodes)
                        {
                            auto path = node.Content.Settlement->GetPathTo(other);
                            if(path == nullptr)
                                continue;
                        
                            auto penalty = path->Complexity;
                            if(node.Content.Distance + penalty < bestComplexity)
                            {
                                bestComplexity = node.Content.Distance + penalty;
                                bestNode = &node;
                                bestSettlement = other;
                            }
                        }
                    }
                }
            }
        }

        auto newNode = bestNode->AddNode({bestSettlement, bestComplexity});
        championPath = newNode;
        bestSettlement->GetPathData().IsVisited = true;
        *visitedSettlements.Add() = bestSettlement;

        if(championPath->Content.Settlement == end)
        {
            break;
        }
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - startClock);
    //std::cout <<"settlement path find duration " << duration.count() << "\n";

    auto complexity = championPath->Content.Distance;
    visitedSettlements.Reset();
    while(true)
    {
        *visitedSettlements.Add() = championPath->Content.Settlement;

        championPath = championPath->GetPrevious();
        if(championPath == nullptr)
        {
            return {visitedSettlements, complexity, visitedSettlements.GetSize()};
        }
    }
}