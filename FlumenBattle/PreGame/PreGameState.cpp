#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Thread/ThreadManager.h"
#include "FlumenEngine/Utility/Perlin.hpp"

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
    static const auto DEFAULT_MENU_OPACITY = Opacity(0.85f);

    PreGameState::PreGameState()
    {
        canvas = ElementFactory::BuildCanvas();

        mainMenu = ElementFactory::BuildElement<MainMenu>(
            {Size(480, 300), DrawOrder(3), {Position2(), canvas}, {"Sprite"}, DEFAULT_MENU_OPACITY}
        );
        mainMenu->Disable();

        newGameMenu = ElementFactory::BuildElement<NewGameMenu>(
            {Size(480, 300), DrawOrder(3), {Position2(), canvas}, {"Sprite"}, DEFAULT_MENU_OPACITY}
        );
        newGameMenu->Disable();

        newWorldMenu = ElementFactory::BuildElement<NewWorldMenu>(
            {Size(480, 300), DrawOrder(3), {Position2(), canvas}, {"Sprite"}, DEFAULT_MENU_OPACITY}
        );
        newWorldMenu->Disable();

        generatorPopup = ElementFactory::BuildElement<GeneratorPopup>(
            {Size(480, 300), DrawOrder(3), {Position2(), canvas}, {"Sprite"}, DEFAULT_MENU_OPACITY}
        );
        generatorPopup->Disable();

        generatedWorldMenu = ElementFactory::BuildElement<GeneratedWorldMenu>(
            {Size(480, 300), DrawOrder(3), {Position2(), canvas}, {"Sprite"}, DEFAULT_MENU_OPACITY}
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

    static auto perlinNoise = container::Grid <float> (128, 128);

    static auto snowNoise = container::Grid <float> (128, 128);

    void PreGameState::GenerateNewWorld(NewWorldData data)
    {
        generatorPopup->Enable();

        Perlin::Generate(Size(perlinNoise.GetWidth(), perlinNoise.GetHeight()), 0.3f, ContrastThreshold(0.5f), ContrastStrength(4.0f));
        
        Perlin::Download(&perlinNoise);

        Perlin::Generate(Size(snowNoise.GetWidth(), snowNoise.GetHeight()), 0.5f, ContrastThreshold(0.5f), ContrastStrength(2.0f));
        
        Perlin::Download(&snowNoise);

        engine::ThreadManager::Get()->LaunchAsyncThread(
            {this, &PreGameState::FinishWorldGeneration}, 
            &world::WorldGenerator::GenerateWorld, world::WorldGenerator::Get(), 
            data, &perlinNoise, &snowNoise);
    }

    void PreGameState::FinishWorldGeneration()
    {
        generatorPopup->Disable();

        generatedWorldMenu->Enable();
    }

    void PreGameState::StartGame()
    {
        generatedWorldMenu->Disable();

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