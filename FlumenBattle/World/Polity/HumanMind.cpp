#include "SDL2/SDL.h"

#include "FlumenEngine/Core/InputHandler.hpp"

#include "HumanMind.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"

using namespace world::polity;

static const SDL_Scancode BUILD_LIBRARY_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_X;

enum class Commands
{
    BUILD_LIBRARY
};

struct Command
{
    Commands Type;
};

static auto commands = container::Array <Command> (8);

HumanMind::HumanMind()
{
    InputHandler::RegisterEvent(BUILD_LIBRARY_INPUT_KEY, {[] {
        *commands.Add() = {Commands::BUILD_LIBRARY};
    }});
}

void HumanMind::MakeDecision(Polity &polity) const
{
    for(auto &command : commands)
    {
        switch(command.Type)
        {
        case Commands::BUILD_LIBRARY:
            for(auto &settlement : polity.GetSettlements())
            {
                settlement->SetProduction(settlement::ProductionOptions::LIBRARY);
            }
            break;
        }
    }

    commands.Reset();
}