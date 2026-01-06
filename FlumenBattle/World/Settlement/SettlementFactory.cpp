#include "SettlementFactory.h"

#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementAllocator.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Settlement/NameGenerator.h"
#include "FlumenBattle/RaceFactory.h"

using namespace world::settlement;

Array <Color> banners = {
    Color::RED, 
    Color::BLUE, 
    Color::YELLOW, 
    Color::CYAN, 
    Color::MAGENTA, 
    Color::GREEN,

    Color::RED * 0.5f + Color::YELLOW * 0.5f,
    Color::RED * 0.5f + Color::MAGENTA * 0.5f,
    Color::BLUE * 0.5f + Color::MAGENTA * 0.5f,
    Color::GREEN * 0.5f + Color::YELLOW * 0.5f,
    Color::GREEN * 0.5f + Color::CYAN * 0.5f,
    Color::BLUE * 0.5f + Color::CYAN * 0.5f,

    Color::RED * 0.25f + Color::YELLOW * 0.75f,
    Color::RED * 0.25f + Color::MAGENTA * 0.75f,
    Color::BLUE * 0.25f + Color::MAGENTA * 0.75f,
    Color::GREEN * 0.25f + Color::YELLOW * 0.75f,
    Color::GREEN * 0.25f + Color::CYAN * 0.75f,
    Color::BLUE * 0.25f + Color::CYAN * 0.75f,

    Color::RED * 0.7f + Color::YELLOW * 0.25f,
    Color::RED * 0.75f + Color::MAGENTA * 0.25f,
    Color::BLUE * 0.75f + Color::MAGENTA * 0.25f,
    Color::GREEN * 0.75f + Color::YELLOW * 0.25f,
    Color::GREEN * 0.75f + Color::CYAN * 0.25f,
    Color::BLUE * 0.75f + Color::CYAN * 0.25f,
    };

Integer lastBannerIndex = 0;

Settlement* SettlementFactory::Create(SettlementBuildData buildData, bool hasExtraData)
{
    auto settlement = SettlementAllocator::Get()->Allocate(hasExtraData);

    auto banner = banners.Get(lastBannerIndex++);

    auto name = NameGenerator::Get()->GenerateName();

    if(lastBannerIndex == banners.GetCapacity())
    {
        lastBannerIndex = 0;
    }

    auto race = RaceFactory::BuildRace(buildData.Race);

    settlement->Initialize(name, *banner, buildData.Location);

    buildData.Location->Settle(settlement);

    return settlement;
}

void SettlementFactory::TransformIntoDeepSettlement(Settlement *settlement)
{
    if(settlement->IsDeepSettlement() == true)
        return;

    SettlementAllocator::Get()->AllocateExtraData(settlement);

    settlement->Deepen();//.Initialize(settlement->GetPopulationHandler());
}

void SettlementFactory::TransformIntoShallowSettlement(Settlement *settlement)
{
    if(settlement->IsDeepSettlement() == false)
        return;

    SettlementAllocator::Get()->FreeExtraData(settlement);
}