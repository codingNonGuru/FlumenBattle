#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "RealmTab.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void DomainItem::HandleConfigure()
{
    SetSpriteColor(BORDER_COLOR);

    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(10.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Medium"}, TEXT_COLOR}
    );
    nameLabel->Enable();

    SetInteractivity(true);
}

void DomainItem::HandleUpdate()
{
    if(isHovered_ == true)
    {
        SetOpacity(1.0f);
    }
    else
    {
        SetOpacity(0.5f);
    }
}

void DomainItem::Setup(settlement::Settlement *settlement)
{
    this->settlement = settlement;

    nameLabel->Setup(settlement->GetName());
}

void RealmTab::HandleConfigure()
{
    static const auto MAXIMUM_POLITY_SIZE = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_POLITY_SIZE).Integer;

    domainLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {Position2{0.0f, 80.0f}, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}
        },
        MAXIMUM_POLITY_SIZE
    );
    domainLayout->SetDistancing(1, 5.0f);
    domainLayout->Enable();

    domainItems.Initialize(MAXIMUM_POLITY_SIZE);
    for(auto i = 0; i < domainItems.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <DomainItem>
        (
            {
                Size(500, 35), 
                drawOrder_ + 1, 
                {domainLayout}, 
                {"panel-border-001", true}
            }
        );
        item->Enable();

        *domainItems.Allocate() = item;
    }
}

void RealmTab::HandleUpdate()
{
    for(auto &item : domainItems)
        item->Disable();

    const auto playerPolity = WorldScene::Get()->GetPlayerPolity();

    auto item = domainItems.GetStart();
    for(auto &settlement : playerPolity->GetSettlements())
    {
        (*item)->Setup(settlement);
        (*item)->Enable();

        item++;
    }
}