#pragma once

#include "FlumenCore/Observer.h"
#include "FlumenCore/Singleton.h"

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
    class PartySetupMenu;
    struct MemberData;

    class PreGameState : public State, public core::Singleton <PreGameState>
    {
        static PreGameState *instance;

        PreGameController *controller;

        Element *canvas;

        MainMenu *mainMenu;

        NewGameMenu *newGameMenu;

        NewWorldMenu *newWorldMenu;

        GeneratorPopup *generatorPopup;

        GeneratedWorldMenu *generatedWorldMenu;

        PartySetupMenu *partySetupMenu;

        void HandleEnter() override;

        void HandleExit() override;

    public:
        Delegate OnWorldGenerationFinished;

        PreGameState();

        void OpenMainMenu();

        void OpenNewWorldMenu();

        void OpenNewGameMenu();

        void OpenPartySetupMenu();

        void OpenGeneratedWorldMenu();

        void GenerateNewWorld(NewWorldData);

        void FinishWorldGeneration();

        void StartGame();

        const container::Array <MemberData> *GetPartyMemberData();
    };
}