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
#include "FlumenBattle/PreGame/PartySetupMenu.h"
#include "FlumenBattle/World/WorldGenerator.h"
#include "FlumenBattle/World/WorldState.h"
#include "FlumenBattle/PreGame/PartyLoader.h"
#include "FlumenBattle/Config.h"

namespace pregame
{
    static const auto DEFAULT_MENU_OPACITY = Opacity(0.85f);

    static const container::Array <MemberData> *partyMemberData = nullptr;

    PreGameState::PreGameState()
    {
        canvas = ElementFactory::BuildCanvas();

        mainMenu = ElementFactory::BuildElement <MainMenu>(
            {Size(480, 300), DrawOrder(3), {canvas}, {false}, DEFAULT_MENU_OPACITY}
        );

        newGameMenu = ElementFactory::BuildElement <NewGameMenu>(
            {Size(480, 300), DrawOrder(3), {canvas}, {false}, DEFAULT_MENU_OPACITY}
        );

        newWorldMenu = ElementFactory::BuildElement <NewWorldMenu>(
            {Size(480, 300), DrawOrder(3), {canvas}, {false}, DEFAULT_MENU_OPACITY}
        );

        generatorPopup = ElementFactory::BuildElement <GeneratorPopup>(
            {Size(480, 300), DrawOrder(3), {canvas}, {false}, DEFAULT_MENU_OPACITY}
        );

        generatedWorldMenu = ElementFactory::BuildElement <GeneratedWorldMenu>(
            {Size(480, 300), DrawOrder(3), {canvas}, {false}, DEFAULT_MENU_OPACITY}
        );

        partyMemberData = PartyLoader::Get()->LoadDefaultPreset();

        partySetupMenu = ElementFactory::BuildElement <PartySetupMenu>(
            {Size(480, 300), DrawOrder(3), {canvas}, {false}, DEFAULT_MENU_OPACITY}
        );
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

    void PreGameState::OpenPartySetupMenu()
    {
        partySetupMenu->Enable();
    }

    void PreGameState::OpenGeneratedWorldMenu()
    {
        generatedWorldMenu->Enable();
    }

    static const auto MAXIMUM_MAP_SIZE = []
    {
        static const auto size = engine::ConfigManager::Get()->GetValue(game::ConfigValues::DEFAULT_WORLD_SIZE).Integer;

        return size;
    } ();

    static auto perlinNoise = container::Grid <float> (MAXIMUM_MAP_SIZE, MAXIMUM_MAP_SIZE);

    static auto snowNoise = container::Grid <float> (MAXIMUM_MAP_SIZE, MAXIMUM_MAP_SIZE);

    static auto desertNoise = container::Grid <float> (MAXIMUM_MAP_SIZE, MAXIMUM_MAP_SIZE);

    static auto forestNoise = container::Grid <float> (MAXIMUM_MAP_SIZE, MAXIMUM_MAP_SIZE);

    void PreGameState::GenerateNewWorld(NewWorldData data)
    {
        generatorPopup->Enable();

        Perlin::Generate(Size(perlinNoise.GetWidth(), perlinNoise.GetHeight()), 0.2f, ContrastThreshold(0.5f), ContrastStrength(2.0f));
        
        Perlin::Download(&perlinNoise);

        Perlin::Generate(Size(snowNoise.GetWidth(), snowNoise.GetHeight()), 0.5f, ContrastThreshold(0.5f), ContrastStrength(2.0f));
        
        Perlin::Download(&snowNoise);

        Perlin::Generate(Size(desertNoise.GetWidth(), desertNoise.GetHeight()), 0.4f, ContrastThreshold(0.5f), ContrastStrength(2.0f));
        
        Perlin::Download(&desertNoise);

        Perlin::Generate(Size(forestNoise.GetWidth(), forestNoise.GetHeight()), 0.6f, ContrastThreshold(0.5f), ContrastStrength(4.0f));
        
        Perlin::Download(&forestNoise);

        engine::ThreadManager::Get()->LaunchAsyncThread(
            {this, &PreGameState::FinishWorldGeneration}, 
            &world::WorldGenerator::GenerateWorld, world::WorldGenerator::Get(), 
            data, &perlinNoise, &snowNoise, &desertNoise, &forestNoise);
    }

    void PreGameState::FinishWorldGeneration()
    {
        generatorPopup->Disable();

        generatedWorldMenu->Enable();

        OnWorldGenerationFinished.Invoke();
    }

    void PreGameState::StartGame()
    {
        world::WorldGenerator::Get()->GeneratePlayerGroup(*partyMemberData);

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

    const container::Array <MemberData> *PreGameState::GetPartyMemberData()
    {
        return partyMemberData;
    }
}