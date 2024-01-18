#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Thread/ThreadManager.h"

#include "PreGameState.h"
#include "FlumenBattle/PreGame/PreGameController.h"
#include "FlumenBattle/PreGame/MainMenu.h"
#include "FlumenBattle/PreGame/NewGameMenu.h"
#include "FlumenBattle/PreGame/NewWorldMenu.h"
#include "FlumenBattle/PreGame/GeneratorPopup.h"
#include "FlumenBattle/PreGame/GeneratedWorldMenu.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/WorldState.h"

namespace pregame
{
    PreGameState::PreGameState()
    {
        canvas = ElementFactory::BuildCanvas();

        mainMenu = ElementFactory::BuildElement<MainMenu>(
            {Size(480, 300), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {"Sprite"}, Opacity(0.85f)}
        );
        mainMenu->Disable();

        newGameMenu = ElementFactory::BuildElement<NewGameMenu>(
            {Size(480, 300), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {"Sprite"}, Opacity(0.85f)}
        );
        newGameMenu->Disable();

        newWorldMenu = ElementFactory::BuildElement<NewWorldMenu>(
            {Size(480, 300), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {"Sprite"}, Opacity(0.85f)}
        );
        newWorldMenu->Disable();

        generatorPopup = ElementFactory::BuildElement<GeneratorPopup>(
            {Size(480, 300), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {"Sprite"}, Opacity(0.85f)}
        );
        generatorPopup->Disable();

        generatedWorldMenu = ElementFactory::BuildElement<GeneratedWorldMenu>(
            {Size(480, 300), DrawOrder(3), {Position2(0.0f, 0.0f), canvas}, {"Sprite"}, Opacity(0.85f)}
        );
        generatedWorldMenu->Disable();
    }

    void PreGameState::OpenMainMenu()
    {
        mainMenu->Enable();
    }

    void PreGameState::OpenNewGameMenu()
    {
        newGameMenu->Enable();
    }

    void PreGameState::OpenNewWorldMenu()
    {
        newWorldMenu->Enable();
    }

    void PreGameState::GenerateNewWorld(NewWorldData data)
    {
        generatorPopup->Enable();

        engine::ThreadManager::Get()->LaunchAsyncThread(
            {this, &PreGameState::FinishWorldGeneration}, 
            &world::WorldGenerator::GenerateWorld, world::WorldGenerator::Get(), 
            data);
    }

    void PreGameState::FinishWorldGeneration()
    {
        generatorPopup->Disable();

        generatedWorldMenu->Enable();
    }

    void PreGameState::StartGame()
    {
        world::WorldState::Get()->Enter();
    }

    void PreGameState::HandleEnter()
    {
        PreGameController::Get()->Enable();

        canvas->Enable();

        OpenMainMenu();
    }

    void PreGameState::HandleExit() 
    {
        PreGameController::Get()->Disable();

        canvas->Disable();
    }
}