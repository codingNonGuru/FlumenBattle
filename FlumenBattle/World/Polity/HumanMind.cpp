#include "SDL2/SDL.h"

#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Interface/Element.hpp"

#include "HumanMind.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Science/Technology.h"

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

void HumanMind::EnableInput()
{
    static const auto canvas = WorldInterface::Get()->GetCanvas();
    canvas->GetLeftClickEvents() += {this, &HumanMind::HandleWorkerPlacement};
}

void HumanMind::DisableInput()
{
    static const auto canvas = WorldInterface::Get()->GetCanvas();
    canvas->GetLeftClickEvents() -= {this, &HumanMind::HandleWorkerPlacement};
}

void HumanMind::MakeDecision(Polity &polity) const
{
    /*for(auto &command : commands)
    {
        if(command.Settlement->GetCurrentProduction()->Is(command.Type) == true)
            continue;

        command.Settlement->SetProduction(command.Type);
    }

    commands.Reset();*/
}

void HumanMind::UpdateWorkforce(Polity &polity) const
{

}

void HumanMind::DecideResearch(Polity &polity) const
{

}

void HumanMind::SetResearchTarget(science::Technologies tech)
{
    const auto playerPolity = WorldScene::Get()->GetPlayerPolity();

    auto techRoster = playerPolity->technologyRoster;

    techRoster->StartResearching(tech);
}

void HumanMind::ProcessProductionInput(settlement::ProductionOptions option, settlement::Settlement *settlement)
{
    if(settlement->GetCurrentProduction()->Is(option) == true)
        return;

    settlement->SetProduction(option);

    OnProductionDecided.Invoke();

    /*for(auto &command : commands)
    {
        if(command.Type == option && command.Settlement == settlement)
            return;
    }

    *commands.Add() = {option, settlement};*/
}

void HumanMind::HandleWorkerPlacement()
{
    if(WorldController::Get()->IsWorkerPlaceModeActive() == false)
        return;

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = playerGroup->GetCurrentSettlement();

    const auto hoveredTile = WorldController::Get()->GetHoveredTile();
    if(hoveredTile->GetOwner() != playerSettlement)
        return;

    for(auto &tile : playerSettlement->GetTiles())
    {
        if(tile.Tile != hoveredTile)
            continue;

        if(tile.Tile == playerSettlement->GetLocation())
            continue;

        if(tile.IsWorked == true)
        {
            tile.IsWorked = false;
        }
        else if(playerSettlement->GetFreeWorkerCount() > 0)
        {
            tile.IsWorked = true;
        }
    }
}

settlement::Shipment currentShipment;

void HumanMind::ProcessTrade(Polity &polity) const
{
    for(auto &settlement : polity.GetSettlements())
    {
        auto &shipment = settlement->GetLastOutgoingShipment();

        if(shipment.TimeInAbsoluteTicks == WorldScene::Get()->GetTime().TotalMinuteCount)
        {
            currentShipment = shipment;

            OnTradeShipment.Invoke();
        }
    }
}

const settlement::Shipment &HumanMind::GetCurrentShipment()
{
    return currentShipment;
}