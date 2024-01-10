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

static Array <Word> names = {"Safehaven", "Ironforge", "Grimwood", "Frostmane", "Billham", "Stoneground", "Hallmark"};

Integer lastBannerIndex = 0;

Integer lastNameIndex = 0;

Settlement* SettlementFactory::Create(SettlementBuildData buildData)
{
    auto settlement = SettlementAllocator::Get()->Allocate();

    auto banner = banners.Get(lastBannerIndex++);

    auto name = names.Get(lastNameIndex++);

    if(lastBannerIndex == banners.GetCapacity())
    {
        lastBannerIndex = 0;
    }

    if(lastNameIndex == names.GetCapacity())
    {
        lastNameIndex = 0;
    }

    settlement->Initialize(*name, *banner, buildData.Location);

    buildData.Location->Settle(settlement);

    return settlement;
}