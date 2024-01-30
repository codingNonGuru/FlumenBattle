#pragma once

#include "FlumenCore/Observer.h"

#include "FlumenEngine/Core/State.hpp"

#include "FlumenBattle/PreGame/Types.h"

class Element;

namespace world
{
    class World;
}

namespace pregame
{
    class PreGameController;
    class MainMenu;
    class NewGameMenu;
    class NewWorldMenu;
    class GeneratorPopup;
    class GeneratedWorldMenu;

    class PreGameState : public State
    {
        PreGameController *controller;

        Element *canvas;

        MainMenu *mainMenu;

        NewGameMenu *newGameMenu;

        NewWorldMenu *newWorldMenu;

        GeneratorPopup *generatorPopup;

        GeneratedWorldMenu *generatedWorldMenu;

        PreGameState();

        void HandleEnter() override;

        void HandleExit() override;

    public:
        Delegate OnWorldGenerationFinished;

        void OpenMainMenu();

        void OpenNewWorldMenu();

        void OpenNewGameMenu();

        void GenerateNewWorld(NewWorldData);

        void FinishWorldGeneration();

        void StartGame();

        static PreGameState * Get()
        {
            static PreGameState state;

            return &state;
        }
    };
}