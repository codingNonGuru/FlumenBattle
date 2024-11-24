#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "PartySetupMenu.h"
#include "FlumenBattle/PreGame/PreGameState.h"
#include "FlumenBattle/PreGame/PartyLoader.h"
#include "FlumenBattle/World/Character/ClassFactory.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/RaceFactory.h"
#include "FlumenBattle/Config.h"

using namespace pregame;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto BORDER_INSET = Size(4, 4);

static const auto DEFAULT_MENU_OPACITY = Opacity(0.85f);

static const auto VISIBLE_ITEM_COUNT = 6;

static const auto ITEM_SIZE = Size(120, 180);

static const auto ITEM_DISTANCE = 10.0f;

static const auto ITEM_LAYOUT_OFFSET = Position2(30.0f, 0.0f);

void PartySetupMenu::HandleConfigure()
{
    startGameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, -60.0f), this}},
        {{"Large"}, TEXT_COLOR, "[S]tart Game"}
    );
    startGameLabel->Enable();

    savePresetLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, -20.0f), this}},
        {{"Large"}, TEXT_COLOR, "Sa[v]e Preset"}
    );
    savePresetLabel->Enable();

    loadPresetLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 20.0f), this}},
        {{"Large"}, TEXT_COLOR, "[L]oad Preset"}
    );
    loadPresetLabel->Enable();

    backLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 60.0f), this}},
        {{"Large"}, TEXT_COLOR, "[B]ack"}
    );
    backLabel->Enable();

    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - BORDER_INSET, 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true}
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    auto width = ITEM_SIZE.x * VISIBLE_ITEM_COUNT + ITEM_DISTANCE * (VISIBLE_ITEM_COUNT - 1) + 2 * ITEM_LAYOUT_OFFSET.x;

    setupPanel = ElementFactory::BuildElement <PartySetupPanel>(
        {Size(width, 300), DrawOrder(3), {Position2(0.0f, 30.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, this}, {false}, DEFAULT_MENU_OPACITY}
    );
    setupPanel->Enable();

    const auto partyMemberData = PreGameState::Get()->GetPartyMemberData();

    setupPanel->Setup(*partyMemberData);
}

void PartySetupMenu::HandleEnable()
{
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_S, {this, &PartySetupMenu::OnStartGamePressed});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_B, {this, &PartySetupMenu::OnBackPressed});
}

void PartySetupMenu::HandleDisable()
{
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_S);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_B);
}

void PartySetupMenu::OnStartGamePressed()
{
    Disable();

    PreGameState::Get()->StartGame();
}

void PartySetupMenu::OnBackPressed()
{
    Disable();

    PreGameState::Get()->OpenGeneratedWorldMenu();
}

void PartySetupPanel::HandleConfigure()
{
    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - BORDER_INSET, 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true},
            Opacity(1.0f)
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    static const auto itemCount = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_CHARACTERS_PER_GROUP).Integer;

    itemLayout = ElementFactory::BuildElement <LayoutGroup> 
    (
        {drawOrder_, {ITEM_LAYOUT_OFFSET, ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}
    );
    itemLayout->SetDistancing(VISIBLE_ITEM_COUNT, ITEM_DISTANCE);
    itemLayout->Enable();

    itemLayout->MakeScrollable(VISIBLE_ITEM_COUNT, VISIBLE_ITEM_COUNT);

    items.Initialize(itemCount);
    for(int i = 0; i < items.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <PartyMemberItem>
        (
            {ITEM_SIZE, drawOrder_ + 1, {itemLayout}, {"panel-border-031", true}, DEFAULT_MENU_OPACITY}
        );
        item->SetSpriteColor(BORDER_COLOR);

        *items.Allocate() = item;
    }
}

void PartySetupPanel::Setup(const container::Array <MemberData> &memberDatas)
{
    for(auto &item : items)
    {
        item->Disable();
    }

    auto item = items.GetStart();
    for(auto &data : memberDatas)
    {
        (*item)->Setup(data);
        (*item)->Enable();

        //

        item++;
    }

    itemLayout->SetScrollableChildCount(memberDatas.GetSize());
}

void PartyMemberItem::HandleConfigure()
{
    classLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, -40.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}},
        {{"Small"}, TEXT_COLOR, ""}
    );
    classLabel->Enable();

    raceLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, -20.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}},
        {{"Small"}, TEXT_COLOR, ""}
    );
    raceLabel->Enable();
}

void PartyMemberItem::Setup(MemberData data)
{
    const auto className = world::character::ClassFactory::BuildClass(data.Class).Name;
    classLabel->Setup(className);

    const auto raceName = RaceFactory::BuildRace(data.Race)->Name;
    raceLabel->Setup(raceName);
}