#include "SettlementFactory.h"

#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/WorldTile.h"

using namespace world::settlement;

Array <Color> banners = {
    Color::RED, 
    Color::BLUE, 
    Color::YELLOW, 
    Color::CYAN, 
    Color::MAGENTA, 
    Color::RED * 0.5f + Color::YELLOW * 0.5f,
    Color::GREEN};

static Array <Word> names = {"Safehaven", "Ironforge", "Grimwood", "Frostmane", "Billham", "Stoneground", "Hallmark"};

Integer lastBannerIndex = 0;

Settlement* SettlementFactory::Create(SettlementBuildData buildData)
{
    auto settlement = SettlementAllocator::Get()->Allocate();

    auto banner = banners.Get(lastBannerIndex);

    auto name = names.Get(lastBannerIndex++);

    if(lastBannerIndex == names.GetCapacity())
    {
        lastBannerIndex = 0;
    }

    settlement->Initialize(*name, *banner, buildData.Location);

    buildData.Location->Settle(settlement);

    return settlement;
}