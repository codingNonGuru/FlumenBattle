#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "GeneratedWorldMenu.h"
#include "FlumenBattle/PreGame/PreGameState.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Tile/WorldMap.h"
#include "FlumenBattle/World/WorldGenerator.h"

using namespace pregame;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto BORDER_INSET = Size(4, 4);

void GeneratedWorldMenu::HandleConfigure() 
{
    startGameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, -60.0f), this}},
        {{"Large"}, TEXT_COLOR, "[S]tart Game"}
    );
    startGameLabel->Enable();

    changePartyLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, -20.0f), this}},
        {{"Large"}, TEXT_COLOR, "[C]hange Party"}
    );
    changePartyLabel->Enable();

    saveWorldLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 20.0f), this}},
        {{"Large"}, TEXT_COLOR, "[S]ave World"}
    );
    saveWorldLabel->Enable();

    backLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 60.0f), this}},
        {{"Large"}, TEXT_COLOR, "[B]ack"}
    );
    backLabel->Enable();

    backdrop = ElementFactory::BuildElement <Element>(
        {Size(300, 300), drawOrder_, {Position2(0.0f, 10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, this}, {false}, opacity_}
    );
    backdrop->Enable();

    sizeLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 5.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, backdrop}},
        {{"Medium"}, TEXT_COLOR, "World size: 50"}
    );
    //sizeLabel->SetAlignment(Text::Alignments::LEFT);
    sizeLabel->Enable();

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
}

void GeneratedWorldMenu::HandleEnable()
{
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_S, {this, &GeneratedWorldMenu::OnStartGamePressed});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_C, {this, &GeneratedWorldMenu::OnChangePartyPressed});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_B, {this, &GeneratedWorldMenu::OnBackPressed});

    auto scene = world::WorldScene::Get();
    auto generator = world::WorldGenerator::Get();
    auto map = scene->GetWorldMap();

    Phrase text;
    text << "World size: " << map->GetSize() << "\n";
    text << "Tile count: " << map->GetTileCount() << "\n";
    text << "Settlement limit: " << generator->GetMaximumSettlementCount(map->GetSize()) << "\n";
    text << "Polity limit: " << generator->GetMaximumPolityCount(map->GetSize()) << "\n";
    text << "Group limit: " << generator->GetMaximumGroupCount(map->GetSize()) << "\n";
    sizeLabel->Setup(text);
}

void GeneratedWorldMenu::HandleDisable() 
{
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_S);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_B);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_C);
}

void GeneratedWorldMenu::OnStartGamePressed()
{
    PreGameState::Get()->StartGame();
}

void GeneratedWorldMenu::OnChangePartyPressed()
{
    Disable();

    PreGameState::Get()->OpenPartySetupMenu();
}

void GeneratedWorldMenu::OnBackPressed()
{
    std::cout<<"aloha\n";
    this->Disable();
}