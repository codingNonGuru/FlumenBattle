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
#include "FlumenBattle/World/Polity/WorkInstruction.h"
#include "FlumenBattle/World/Settlement/Building.h"

using namespace world;
using namespace world::polity;

struct Command
{
    settlement::ProductionOptions Type;

    settlement::Settlement *Settlement;
};

#define MAXIMUM_COMMAND_COUNT 8

#define INSTRUCTIONS_PER_SETTLEMENT 64

#define INSTRUCTION_SETS_PER_POLITY 128

static auto commands = container::Array <Command> (MAXIMUM_COMMAND_COUNT);

static auto workInstructionSets = container::Array <InstructionSet> (INSTRUCTION_SETS_PER_POLITY);

HumanMind::HumanMind()
{
    for(int i = 0; i < workInstructionSets.GetCapacity(); ++i)
    {
        auto set = workInstructionSets.Add();
        set->settlement = nullptr;
        set->instructions.Initialize(INSTRUCTIONS_PER_SETTLEMENT);
    }

    workInstructionSets.Reset();
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
    UpdateWorkforce(polity);

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
    for(auto &settlement : polity.GetSettlements())
    {
        UpdateSettlementWorkforce(settlement);
    }
}

void HumanMind::UpdateSettlementWorkforce(settlement::Settlement *settlement) const
{
    auto instructionSet = workInstructionSets.Find(settlement);
    if(instructionSet == nullptr)
    {
        return;
    }

    settlement->FireAllWorkers();

    static auto workQueue = container::Block <WorkInstruction *, INSTRUCTIONS_PER_SETTLEMENT> ();

    for(auto &instruction : instructionSet->instructions)
    {
        *workQueue[instruction.Priority] = &instruction;
    }

    auto freeWorkerCount = settlement->GetFreeWorkerCount();
    for(int i = 0; i < instructionSet->instructions.GetSize(); ++i)
    {
        if(freeWorkerCount == 0)
        {
            break;
        }

        auto instruction = *workQueue[i];

        if(instruction->PlaceType == WorkInstruction::BUILDING)
        {
            auto hasHired = settlement->HireWorker(instruction->Place.Building);
            if(hasHired == true)
            {
                freeWorkerCount--;
            }
        }
        else if(instruction->PlaceType == WorkInstruction::TILE)
        {
            instruction->Place.Tile->IsWorked = true;

            freeWorkerCount--;
        }
    }
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

    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    const auto hoveredTile = WorldController::Get()->GetHoveredTile();
    if(hoveredTile->GetOwner() != playerSettlement)
        return;

    auto instructionSet = workInstructionSets.Find(playerSettlement);
    if(instructionSet == nullptr)
    {
        auto tile = playerSettlement->GetTiles().Find(hoveredTile);
        if(tile->Tile != playerSettlement->GetLocation())
        {
            HireWorker(playerSettlement, tile);
        }
    }
    else
    {
        auto tile = playerSettlement->GetTiles().Find(hoveredTile);
        if(tile->Tile != playerSettlement->GetLocation())
        {
            auto instruction = instructionSet->instructions.Find(tile);
            if(instruction == nullptr)
            {
                HireWorker(playerSettlement, tile);    
            }
            else
            {
                FireWorker(playerSettlement, tile);    
            }
        }
    }

    UpdateSettlementWorkforce(playerSettlement);

    OnTileWorkerChanged.Invoke();
}

settlement::Shipment currentShipment;

void HumanMind::ProcessTrade(Polity &polity) const
{
    for(auto &settlement : polity.GetSettlements())
    {
        auto &shipment = settlement->GetLastOutgoingShipment();

        if(shipment.TimeInAbsoluteTicks == WorldScene::Get()->GetTime().GetTickCount())
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

void HumanMind::HireWorker(settlement::Settlement *settlement, settlement::SettlementTile *tile)
{
    auto instructionSet = workInstructionSets.Find(settlement);
    if(instructionSet == nullptr)
    {
        instructionSet = workInstructionSets.Add();
        instructionSet->settlement = settlement;
        instructionSet->instructions.Reset();
    }

    int priority = instructionSet->instructions.GetSize();
    auto instruction = instructionSet->instructions.Add();

    *instruction = {priority, WorkInstruction::TILE, {tile}};
}

void HumanMind::HireWorker(settlement::Settlement *settlement, settlement::Building *building)
{
    auto instructionSet = workInstructionSets.Find(settlement);
    if(instructionSet == nullptr)
    {
        instructionSet = workInstructionSets.Add();
        instructionSet->settlement = settlement;
        instructionSet->instructions.Reset();
    }

    int priority = instructionSet->instructions.GetSize();
    auto instruction = instructionSet->instructions.Add();

    *instruction = {priority, WorkInstruction::BUILDING, {building}};
}

void HumanMind::FireWorker(settlement::Settlement *settlement, settlement::SettlementTile *tile)
{
    auto instructionSet = workInstructionSets.Find(settlement);

    auto instruction = instructionSet->instructions.Find(tile);
    auto priority = instruction->Priority;

    for(auto &instruction : instructionSet->instructions)
    {
        if(instruction.Priority > priority)
        {
            instruction.Priority--;
        }
    }

    instructionSet->instructions.RemoveAt(instruction);
}

void HumanMind::FireWorker(settlement::Settlement *settlement, settlement::Building *building)
{
    auto instructionSet = workInstructionSets.Find(settlement);

    auto instruction = instructionSet->instructions.Find(building);
    auto priority = instruction->Priority;

    for(auto &instruction : instructionSet->instructions)
    {
        if(instruction.Priority > priority)
        {
            instruction.Priority--;
        }
    }

    instructionSet->instructions.RemoveAt(instruction);
}

void HumanMind::ChangeBuildingWorkforce(settlement::Building *building, bool isHiring)
{
    if(building->GetOutputResource().Resource == world::settlement::ResourceTypes::NONE)
        return;

    auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    auto instructionSet = workInstructionSets.Find(playerSettlement);
    if(instructionSet == nullptr)
    {
        instructionSet = workInstructionSets.Add();
        instructionSet->settlement = playerSettlement;
        instructionSet->instructions.Reset();
    }

    auto plannedEmployeeCount = instructionSet->instructions.GetCount(building);

    if(isHiring == true)
    {
        if(plannedEmployeeCount < building->GetAmount())
        {
            HireWorker(playerSettlement, building);
        }
    }
    else
    {
        if(plannedEmployeeCount > 0)
        {
            FireWorker(playerSettlement, building);
        }
    }

    UpdateSettlementWorkforce(playerSettlement);
}

const container::Pool <WorkInstruction> *HumanMind::GetSettlementInstructions() const
{
    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    const auto playerSettlement = playerGroup->GetCurrentSettlement();

    if(auto set = workInstructionSets.Find(playerSettlement); set == nullptr)  
    {
        return nullptr;
    }
    else
    {
        return &set->instructions;
    }
}