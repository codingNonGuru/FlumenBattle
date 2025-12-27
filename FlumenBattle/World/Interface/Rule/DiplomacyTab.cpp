#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/SimpleList.h"

#include "DiplomacyTab.h"
#include "NeighborItem.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Polity/Neighbor.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Config.h"

using namespace world::interface::rule;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto BORDER_COLOR = Color::RED * 0.25f;

void DiplomacyTab::HandleConfigure()
{
    static const auto NEIGHBORS_PER_POLITY = engine::ConfigManager::Get()->GetValue(game::ConfigValues::NEIGHBORS_PER_POLITY).Integer;

    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR}
    );
    nameLabel->Disable();

    neighborItemLayout = ElementFactory::BuildSimpleList <SimpleList>
    (
        { 
            drawOrder_,
            {Position2{10.0f, 60.0f}, ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}
        },
        NEIGHBORS_PER_POLITY,
        5.0f
    );
    neighborItemLayout->AddScroller(8, neighborItems.GetCapacity());
    neighborItemLayout->Enable();

    neighborItems.Initialize(NEIGHBORS_PER_POLITY);
    for(auto i = 0; i < neighborItems.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <NeighborItem>
        (
            {
                Size(400, 40), 
                drawOrder_ + 2, 
                {neighborItemLayout}, 
                {"panel-border-001", true}
            }
        );
        item->SetSpriteColor(BORDER_COLOR);

        *neighborItems.Allocate() = item;
    }
}

void DiplomacyTab::HandleUpdate()
{
    static const auto ruleMenu = WorldInterface::Get()->GetRuleMenu();

    const auto playerPolity = WorldScene::Get()->GetPlayerPolity();

    auto item = neighborItems.GetStart();
    for(auto &neighbor : playerPolity->GetNeighbors())
    {
        (*item)->Setup(&neighbor, this);

        (*item)->SetInteractivity(true);

        (*item)->Enable();
        
        item++;
    }

    while(item != neighborItems.GetEnd())
    {
        (*item)->Disable();

        item++;
    }

    //nameLabel->Setup(settlement->GetName());
}