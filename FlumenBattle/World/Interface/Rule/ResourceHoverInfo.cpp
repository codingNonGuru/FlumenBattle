#include "FlumenEngine/Interface/ElementFactory.h"

#include "ResourceHoverInfo.h"
#include "EconomyTab.h"
#include "ResourceItem.h"
#include "RaceItem.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/RaceFactory.h"
#include "FlumenBattle/Config.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto HIGHLIGHT_COLOR = Color::RED * 0.8f;

static const SpriteDescriptor BORDER_SPRITE = {"panel-border-007", true};

static const auto MOUSE_FOLLOW_OFFSET = Position2(0.0f, 10.0f);

static const auto BASE_SIZE = Size(100, 100);

static const auto OPACITY = Opacity(0.7f);

static const auto BORDER_OPACITY = Opacity(0.7f);

void ResourceHoverInfo::HandleConfigure()
{
    hoveredItem = nullptr;

    size_ = BASE_SIZE;

    border = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ + 1, {this}, BORDER_SPRITE, BORDER_OPACITY}
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    FollowMouse(MOUSE_FOLLOW_OFFSET);

    SetOpacity(OPACITY);

    raceLabel = ElementFactory::BuildRichText(
        {drawOrder_ + 1, {Position2(0.0f, -25.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, this}}, 
        {{"Small"}, TEXT_COLOR, HIGHLIGHT_COLOR, "<2>G<1>oblin"}
    );
    raceLabel->Enable();

    static const auto RACE_GROUPS_PER_SETTTLEMENT = engine::ConfigManager::Get()->GetValue(game::ConfigValues::RACE_GROUPS_PER_SETTTLEMENT).Integer;

    raceItemGroup = ElementFactory::BuildSimpleList
    (
        { 
            drawOrder_, 
            {Position2{0.0f, 15.0f}, ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, this}, 
            {false},
            Opacity(0.0f)
        },
        RACE_GROUPS_PER_SETTTLEMENT,
        ListOrientations::HORIZONTAL,
        5.0f
    );
    raceItemGroup->Enable();

    raceItems.Initialize(RACE_GROUPS_PER_SETTTLEMENT);

    for(auto i = 0; i < raceItems.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <RaceItem>
        (
            {
                Size(48, 56), 
                drawOrder_ + 1, 
                {raceItemGroup}, 
                {"panel-border-015", true}
            }
        );
        item->Enable();

        *raceItems.Add() = item;
    }

    polity::HumanMind::Get()->OnRaceToEmployShifted += Event{this, &ResourceHoverInfo::HandleRaceShifted};
}

void ResourceHoverInfo::HandleUpdate()
{
    if(isTileBased == false)
    {
        if(hoveredItem == nullptr)
            return;

        if(hoveredItem->IsHovered() == false)
        {
            hoveredItem = nullptr;
            Disable();
            return;
        }
    }
}

void ResourceHoverInfo::Setup(ResourceItem *item)
{
    hoveredItem = item;

    isTileBased = false;

    RefreshItems();
}

void ResourceHoverInfo::Setup()
{
    hoveredItem = nullptr;

    isTileBased = true;

    RefreshItems();
}

void ResourceHoverInfo::HandleRaceShifted()
{
    if(this->IsGloballyActive() == false)
        return;

    RefreshItems();
}

void ResourceHoverInfo::RefreshItems()
{
    for(auto &raceItem : raceItems)
    {
        raceItem->Disable();
    }

    auto raceItem = raceItems.GetStart();

    auto nextRaceToEmploy = polity::HumanMind::Get()->GetNextRaceToEmploy();

    auto &races = polity::HumanMind::Get()->GetUnemployedByRace();
    for(auto &race : races)
    {
        (*raceItem)->Setup(&race, race.Race == nextRaceToEmploy);
        (*raceItem)->Enable();

        raceItem++;
    }

    raceItemGroup->UpdateSize();
    size_.x = raceItemGroup->GetSize().x + 10;

    border->GetSize() = size_;

    auto text = Word() << RaceFactory::BuildRace(nextRaceToEmploy)->Name;
    raceLabel->Setup(Word() << "<2>" << text.GetFirstCharacter() << "<1>" << (text.Get() + 1));
}