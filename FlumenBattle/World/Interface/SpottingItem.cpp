#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "SpottingItem.h"
#include "FlumenBattle/World/Group/GroupSpotting.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupType.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void SpottingItem::HandleConfigure()
{
    size_ = Size(200, 35);

    opacity_ = Opacity(0.6f);

    SetSpriteColor(BORDER_COLOR);

    nameLabel = ElementFactory::BuildText 
    (
        {drawOrder_, {Position2(10.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"Small"}, TEXT_COLOR, ""}
    );
    nameLabel->Enable();
}

void SpottingItem::HandleUpdate()
{
    switch(spotting->Group->GetClass())
    {
        case group::GroupClasses::MERCHANT:
            nameLabel->Setup("Merchants");
            break;
        case group::GroupClasses::ADVENTURER:
            nameLabel->Setup("Adventurers");
            break;
        case group::GroupClasses::BANDIT:
            nameLabel->Setup("Bandits");
            break;
    }
}