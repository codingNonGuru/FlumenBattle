#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "SpottingItem.h"
#include "FlumenBattle/World/Group/GroupSpotting.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Group/GroupType.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto BASE_OPACITY = Opacity(0.5f);

static const auto HOVERED_OPACITY = Opacity(0.9f);

void SpottingItem::HandleConfigure()
{
    opacity_ = BASE_OPACITY;

    SetSpriteColor(BORDER_COLOR);

    nameLabel = ElementFactory::BuildText 
    (
        {drawOrder_, {Position2(5.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}},
        {{"VerySmall"}, TEXT_COLOR, ""}
    );
    nameLabel->Enable();

    timeCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {Position2(-30.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_RIGHT, this}}
    );
    timeCounter->Setup("HourGlass", &time);
    timeCounter->SetOffset(-3.0f);
    timeCounter->Enable();

    skullIcon = ElementFactory::BuildElement <Element>(
        {drawOrder_ + 1, {Position2(-65.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_CENTER, this}, SpriteDescriptor{"Skull", false}}
    );
}

void SpottingItem::HandleUpdate()
{
    auto string = Word() << utility::GetClassName(spotting->GroupClass) << "s";

    string << " of " << spotting->OriginName;

    nameLabel->Setup(string);

    static auto &worldTime = WorldScene::Get()->GetTime();
    time = worldTime.TotalHourCount - spotting->TimeInHours;

    auto group = WorldScene::Get()->GetGroup(spotting->GroupUniqueId);
    if(group != nullptr && group->IsAlive() == true)
    {
        skullIcon->Disable();
    }
    else
    {
        skullIcon->Enable();
    }

    if(isHovered_ == true)
    {
        opacity_ = HOVERED_OPACITY;
    }
    else
    {
        opacity_ = BASE_OPACITY;
    }

    if(isHovered_ == true)
    {
        group::HumanMind::Get()->SetHoveredSpotting(spotting);
    }
    else
    {
        if(group::HumanMind::Get()->GetHoveredSpotting() == spotting)
        {
            group::HumanMind::Get()->SetHoveredSpotting(nullptr);
        }
    }
}

void SpottingItem::HandleLeftClick()
{
    group::HumanMind::Get()->PursueSighting(*spotting);
}

bool SpottingItem::HasContent(const group::GroupSpotting &spotting) 
{
    if(this->spotting == nullptr)
        return false;
        
    return this->spotting->GroupUniqueId == spotting.GroupUniqueId;
}