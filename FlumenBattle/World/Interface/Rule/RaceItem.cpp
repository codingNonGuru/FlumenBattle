#include "FlumenEngine/Utility/Color.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "RaceItem.h"
#include "FlumenBattle/RaceFactory.h"
#include "FlumenBattle/World/Settlement/PopHandler.h"
#include "FlumenBattle/World/Interface/Counter.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

void RaceItem::HandleConfigure()
{
    SetSpriteColor(BORDER_COLOR);

    icon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {Position2(0.0f, 0.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, this}, 
            {"Radish", false}
        }
    );
    icon->Enable();

    sizeCounter = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 2, {Position2(5.0f, -5.0f), ElementAnchors::LOWER_LEFT, ElementPivots::LOWER_LEFT, this}, {"WhiteDotBackdrop", false}}
    );
    sizeCounter->Setup(&groupSize, Scale2(0.8f), "Medium");
    sizeCounter->Enable();
}

void RaceItem::Setup(const settlement::RaceGroup *raceGroup, bool isSelected)
{
    auto race = RaceFactory::BuildRace(raceGroup->Race);
    icon->SetTexture(race->TextureName);

    groupSize = raceGroup->Size;

    if(isSelected == true)
    {
        SetOpacity(0.9f);

        icon->SetTextureScale(1.0f);
    }
    else
    {
        SetOpacity(0.4f);

        icon->SetTextureScale(0.8f);
    }
}