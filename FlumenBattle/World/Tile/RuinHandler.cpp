#include "RuinHandler.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Tile/Ruin.h"
#include "FlumenBattle/Race.h"

using namespace world::tile;

void RuinHandler::Add(settlement::Settlement *settlement, bool isSettlementCenter)
{
    auto ruin = ruins.Add();

    auto extinctionDate = WorldScene::Get()->GetTime().GetTickCount();

    *ruin = {settlement->GetFoundationDate(), extinctionDate, settlement->GetHighestPopulationEver(), settlement->GetRace()->Type, isSettlementCenter};
}

void RuinAllocator::Allocate(container::ArrayAllocator <Ruin> &allocator, RuinHandler &handler) 
{
    handler.ruins.Initialize(allocator);
}