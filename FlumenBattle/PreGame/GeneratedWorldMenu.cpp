#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "GeneratedWorldMenu.h"
#include "FlumenBattle/PreGame/PreGameState.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/WorldMap.h"
#include "FlumenBattle/World/WorldGenerator.h"

using namespace pregame;

void GeneratedWorldMenu::HandleConfigure() 
{
    startGameLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -40.0f), this}},
        {{"Large"}, Color::RED * 0.5f, "[S]tart Game"}
    );
    startGameLabel->Enable();

    saveWorldLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 0.0f), this}},
        {{"Large"}, Color::RED * 0.5f, "[S]ave World"}
    );
    saveWorldLabel->Enable();

    backLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 40.0f), this}},
        {{"Large"}, Color::RED * 0.5f, "[B]ack"}
    );
    backLabel->Enable();

    backdrop = ElementFactory::BuildElement <Element>(
        {Size(300, 300), drawOrder_, {Position2(0.0f, 10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, this}, {"Sprite"}, opacity_}
    );
    backdrop->Enable();

    sizeLabel = ElementFactory::BuildText(
        {Size(150, 50), drawOrder_ + 1, {Position2(0.0f, 5.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, backdrop}},
        {{"Medium"}, Color::RED * 0.5f, "World size: 50"}
    );
    //sizeLabel->SetAlignment(Text::Alignments::LEFT);
    sizeLabel->Enable();
}

void GeneratedWorldMenu::HandleEnable()
{
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_S, {this, &GeneratedWorldMenu::OnStartGamePressed});

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
}

void GeneratedWorldMenu::OnStartGamePressed()
{
    PreGameState::Get()->StartGame();
}

void GeneratedWorldMenu::OnBackPressed()
{
    this->Disable();
}