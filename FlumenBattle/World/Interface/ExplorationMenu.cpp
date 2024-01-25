#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "ExplorationMenu.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Interface/SpottingItem.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto BORDER_INNER_OFFSET = Size(4, 4);

static const auto TOGGLE_MESSAGE = "Press [TAB] to toggle";

void ExplorationMenu::HandleConfigure()
{
    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - BORDER_INNER_OFFSET, 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true} 
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    reliefLabel = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}
        },
        {
            {"Medium"}, 
            TEXT_COLOR, 
            ""
        }
    );
    reliefLabel->Enable();

    ownerLabel = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, reliefLabel}
        },
        {
            {"Small"}, 
            TEXT_COLOR, 
            ""
        }
    );
    ownerLabel->Enable();

    spottingGroup = ElementFactory::BuildElement <LayoutGroup>
    (
        {
            drawOrder_,
            {Position2(10.0f, 70.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}
        }
    );
    spottingGroup->SetDistancing(1, 5.0f);
    spottingGroup->Enable();

    static const auto spottingCount = engine::ConfigManager::Get()->GetValue(game::ConfigValues::GROUP_SPOTTING_LIMIT).Integer;
    spottingItems.Initialize(spottingCount);

    for(int i = 0; i < spottingItems.GetCapacity(); ++i)
    {
        auto itemIterator = spottingItems.Add();
        *itemIterator = ElementFactory::BuildElement <SpottingItem>
        (
            { 
                drawOrder_ + 1, 
                {spottingGroup}, 
                {"panel-border-007", true},
            }
        );
        (*itemIterator)->Disable();
    }

    tabLabel = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {Position2(0.0f, -10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}
        },
        {
            {"VerySmall"}, 
            TEXT_COLOR, 
            TOGGLE_MESSAGE
        }
    );
    tabLabel->Enable();

    *group::HumanMind::Get()->OnSettlementEntered += {this, &ExplorationMenu::HandleSettlementEntered};

    *group::HumanMind::Get()->OnSettlementExited += {this, &ExplorationMenu::HandleSettlementExited};

    *group::HumanMind::Get()->OnGroupSpotted += {this, &ExplorationMenu::HandleGroupSpotted};

    *group::HumanMind::Get()->OnGroupFaded += {this, &ExplorationMenu::HandleGroupFaded};
}

void ExplorationMenu::HandleUpdate()
{
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    auto playerLocation = playerGroup->GetTile();

    auto string = LongWord() << [&]
    {
        if(playerLocation->HasRelief(WorldReliefs::MOUNTAINS))
            return "Mountains";
        
        if(playerLocation->HasBiome(WorldBiomes::STEPPE))
            return "Grasslands";
        else if(playerLocation->HasBiome(WorldBiomes::WOODS))
            return "Woods";
        else if(playerLocation->HasBiome(WorldBiomes::DESERT))
            return "Wasteland";
    } ();

    reliefLabel->Setup(string);

    string = [&] -> LongWord
    {
        if(playerLocation->GetOwner() == nullptr)
            return "Belongs to no one";
        else 
            return LongWord() << "Belongs to " << playerLocation->GetOwner()->GetName();  
    } ();

    ownerLabel->Setup(string);
}

void ExplorationMenu::HandleSettlementEntered()
{
    tabLabel->Enable();
}

void ExplorationMenu::HandleSettlementExited()
{
    tabLabel->Disable();
}

void ExplorationMenu::HandleGroupSpotted()
{
    auto &spotting = group::HumanMind::Get()->GetLatestGroupSpotting();

    for(auto &item : spottingItems)
    {
        if(item->HasContent(&spotting) == true)
            break;

        if(item->HasContent(nullptr) == true)
        {
            item->Setup(&spotting);
            item->Enable();
            break;
        }
    }
}

void ExplorationMenu::HandleGroupFaded()
{

}