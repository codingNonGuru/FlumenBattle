#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "RealmTab.h"
#include "DomainHoverInfo.h"
#include "DomainItem.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/World/Settlement/Building.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto HIGHLIGHT_COLOR = Color::RED * 0.9f;

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
                Size(580, 35), 
                drawOrder_ + 1, 
                {domainLayout}, 
                {"panel-border-001", true}
            }
        );
        item->Enable();

        *domainItems.Allocate() = item;
    }

    hoverInfo = ElementFactory::BuildElement <interface::rule::DomainHoverInfo>
    (
        {
            GetDrawOrder() + 5,
            {ElementAnchors::MIDDLE_CENTER, ElementPivots::UPPER_CENTER, this},
            {false}
        }
    );
}

void RealmTab::HandleUpdate()
{
    for(auto &item : domainItems)
        item->Disable();

    const auto playerPolity = WorldScene::Get()->GetPlayerPolity();

    auto item = domainItems.GetStart();
    for(auto &settlement : playerPolity->GetSettlements())
    {
        (*item)->Setup(settlement, this);
        (*item)->Enable();

        item++;
    }
}