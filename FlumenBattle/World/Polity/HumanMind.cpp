#include "SDL2/SDL.h"

#include "FlumenEngine/Core/InputHandler.hpp"

#include "HumanMind.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"

using namespace world;
using namespace world::polity;

struct Command
{
    settlement::ProductionOptions Type;

    settlement::Settlement *Settlement;
};

#define MAXIMUM_COMMAND_COUNT 8

static auto commands = container::Array <Command> (MAXIMUM_COMMAND_COUNT);

HumanMind::HumanMind()
{
}

void HumanMind::MakeDecision(Polity &polity) const
{
    for(auto &command : commands)
    {
        if(command.Settlement->GetCurrentProduction()->Is(command.Type) == true)
            continue;

        command.Settlement->SetProduction(command.Type);
    }

    commands.Reset();
}

void HumanMind::ProcessProductionInput(settlement::ProductionOptions option, settlement::Settlement *settlement)
{
    for(auto &command : commands)
    {
        if(command.Type == option && command.Settlement == settlement)
            return;
    }

    *commands.Add() = {option, settlement};
}