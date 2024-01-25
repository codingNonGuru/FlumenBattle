#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "SpottingItem.h"
#include "FlumenBattle/World/Group/GroupSpotting.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/Character/Character.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void SpottingItem::HandleConfigure()
{
    size_ = Size(280, 35);

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
    auto string = Word() << [&]
    {
        switch(spotting->Group->GetClass())
        {
            case group::GroupClasses::MERCHANT:
                return "Merchants";
            case group::GroupClasses::ADVENTURER:
                return "Adventurers";
            case group::GroupClasses::BANDIT:
                return "Bandits";
        }
    } ();

    string << " " << spotting->Group->GetLeader()->GetName();

    nameLabel->Setup(string);
}