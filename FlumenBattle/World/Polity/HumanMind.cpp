#include "SDL2/SDL.h"

#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Interface/Element.hpp"

#include "HumanMind.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
#include "FlumenBattle/World/Settlement/SettlementProduction.h"
#include "FlumenBattle/World/Settlement/TileImprovement.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/WorldController.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/Science/Technology.h"
#include "FlumenBattle/World/Polity/WorkInstruction.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/World/Interface/Popup/PopupManager.h"
#include "FlumenBattle/World/WorldUpdateHandler.h"
#include "FlumenBattle/World/Settlement/ExplorationHandler.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"

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

static const auto IMPROVEMENT_SWIPE_LEFT_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_SEMICOLON};

static const auto IMPROVEMENT_SWIPE_RIGHT_KEY = InputHandler::Trigger{SDL_Scancode::SDL_SCANCODE_APOSTROPHE};

static auto commands = container::Array <Command> (MAXIMUM_COMMAND_COUNT);

static auto workInstructionSets = container::Array <InstructionSet> (INSTRUCTION_SETS_PER_POLITY);

struct SettleTarget
{
    settlement::Settlement *Settlement;

    tile::WorldTile *Tile;

    bool operator== (const SettleTarget &other) {return Settlement == other.Settlement && Tile == other.Tile;}

    bool operator== (const settlement::Settlement *settlement) {return Settlement == settlement;}
};

auto settleTargets = container::Pool <SettleTarget> (64);

HumanMind::HumanMind()
{
    for(int i = 0; i < workInstructionSets.GetCapacity(); ++i)
    {
        auto set = workInstructionSets.Add();
        set->settlement = nullptr;
        set->instructions.Initialize(INSTRUCTIONS_PER_SETTLEMENT);
    }

    workInstructionSets.Reset();

    WorldScene::Get()->OnRefreshCycleFinished += {this, &HumanMind::HandleSceneUpdateEnded};
}

void HumanMind::EnableInput()
{
    static const auto canvas = WorldInterface::Get()->GetCanvas();
    canvas->GetLeftClickEvents() += {this, &HumanMind::HandleWorkerPlacement};

    canvas->GetLeftClickEvents() += {this, &HumanMind::HandleBorderExpansion};

    canvas->GetLeftClickEvents() += {this, &HumanMind::HandleTileSettled};

    canvas->GetLeftClickEvents() += {this, &HumanMind::HandleExplorationStarted};

    canvas->GetLeftClickEvents() += {this, &HumanMind::HandleTileImproved};

    InputHandler::RegisterEvent(IMPROVEMENT_SWIPE_LEFT_KEY, {this, &HumanMind::HandleImproveSwipeLeft});
    InputHandler::RegisterEvent(IMPROVEMENT_SWIPE_RIGHT_KEY, {this, &HumanMind::HandleImproveSwipeRight});
}

void HumanMind::DisableInput()
{
    static const auto canvas = WorldInterface::Get()->GetCanvas();
    canvas->GetLeftClickEvents() -= {this, &HumanMind::HandleWorkerPlacement};

    canvas->GetLeftClickEvents() -= {this, &HumanMind::HandleBorderExpansion};

    canvas->GetLeftClickEvents() -= {this, &HumanMind::HandleTileSettled};

    canvas->GetLeftClickEvents() -= {this, &HumanMind::HandleExplorationStarted};

    canvas->GetLeftClickEvents() -= {this, &HumanMind::HandleTileImproved};

    InputHandler::UnregisterEvent(IMPROVEMENT_SWIPE_LEFT_KEY);
    InputHandler::UnregisterEvent(IMPROVEMENT_SWIPE_RIGHT_KEY);
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

void HumanMind::HandleBorderExpansion()
{
    if(WorldController::Get()->IsBorderExpandActive() == false)
        return;

    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    const auto hoveredTile = WorldController::Get()->GetHoveredTile();
    if(playerSettlement->CanExpandHere(hoveredTile) == false)
        return;

    if(playerSettlement->CanAffordToExpandHere(hoveredTile) == false)
        return;

    hoveredTile->AssertOwnership(playerSettlement);

    auto newTile = playerSettlement->tiles.Add();
    newTile->Tile = hoveredTile;
    newTile->ResetImprovement();
    newTile->IsWorked = false;

    playerSettlement->cultureGrowth -= playerSettlement->GetExpansionCost(hoveredTile);

    playerSettlement->RemoveFinishedExploration(hoveredTile);

    OnPlayerSettlementBorderExpanded.Invoke();
}

void HumanMind::HandleTileSettled()
{
    if(WorldController::Get()->IsSettleModeActive() == false)
        return;

    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    const auto hoveredTile = WorldController::Get()->GetHoveredTile();
    if(playerSettlement->CanSettleHere(hoveredTile) == false)
        return;

    if(playerSettlement->HasAnySettlers() == true)
    {
        WorldScene::Get()->FoundSettlement(hoveredTile, playerSettlement->GetRace()->Type, playerSettlement);

        OnPlayerSettlementColonized.Invoke();

        playerSettlement->RemoveSettlers();
        return;
    }
        
    auto data = settleTargets.Find(SettleTarget{playerSettlement, hoveredTile});
    if(data == nullptr)
    {
        auto otherTile = settleTargets.Find(playerSettlement);
        if(otherTile != nullptr)
        {
            settleTargets.RemoveAt(otherTile);
        }
        
        *settleTargets.Add() = {playerSettlement, hoveredTile};

        if(playerSettlement->GetCurrentProduction()->GetType() != settlement::ProductionOptions::SETTLERS)
        {
            playerSettlement->SetProduction(settlement::ProductionOptions::SETTLERS);
        }
    }
    else
    {
        settleTargets.RemoveAt(data);
    }
}

void HumanMind::HandleExplorationStarted()
{
    if(WorldController::Get()->IsExploreModeActive() == false)
        return;

    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    const auto hoveredTile = WorldController::Get()->GetHoveredTile();
    if(playerSettlement->CanExploreHere(hoveredTile) == false)
        return;

    if(playerSettlement->HasExplored(hoveredTile) == true)
        return;

    if(playerSettlement->IsExploring() == true)
    {
        playerSettlement->StopExploring();
    }
    else
    {
        playerSettlement->StartExploring(hoveredTile);
    }

    OnExplorationStarted.Invoke();
}

auto improvementTypeIndex = 0; 

void HumanMind::HandleImproveSwipeLeft()
{
    improvementTypeIndex--;

    if(improvementTypeIndex < 0)
    {
        improvementTypeIndex = (int)settlement::TileImprovements::NONE - 1;
    }
}

void HumanMind::HandleImproveSwipeRight()
{
    improvementTypeIndex++;

    if(improvementTypeIndex == (int)settlement::TileImprovements::NONE)
    {
        improvementTypeIndex = 0;
    }
}

void HumanMind::HandleTileImproved()
{
    if(WorldController::Get()->IsTileDevelopModeActive() == false)
        return;

    const auto playerSettlement = WorldScene::Get()->GetPlayerSettlement();

    const auto improvementType = settlement::TileImprovementFactory::Get()->BuildImprovementType(settlement::TileImprovements(improvementTypeIndex));

    const auto hoveredTile = WorldController::Get()->GetHoveredTile();
    if(playerSettlement->CanImproveHere(hoveredTile, improvementType->Type) == false)
        return;

    if(playerSettlement->IsImprovingTile(hoveredTile, improvementType->Type) == true)
        return;

    if(playerSettlement->IsImprovingAnyTile() == true)
    {
        playerSettlement->CancelImproving();
    }
    else
    {
        playerSettlement->StartImprovingTile(hoveredTile, improvementType->Type);
    }

    OnImprovementStarted.Invoke();
}

void HumanMind::HandleSceneUpdateEnded()
{
    const auto playerPolity = WorldScene::Get()->GetPlayerPolity();

    if(playerPolity != nullptr)
    {
        for(auto &settlement : playerPolity->GetSettlements())
        {
            for(auto &tile : settlement->GetTiles())
            {
                tile.Tile->RevealTile();
            }

            for(auto &group : settlement->GetGroupDynamics().GetGroups())
            {
                if(group->GetTile() == nullptr)
                    continue;

                if(group->GetClass() == group::GroupClasses::BANDIT)
                    continue;

                group->GetTile()->RevealTile();
            }
        }
    }

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->GetTile() != nullptr)
    {
        playerGroup->GetTile()->RevealTile();
    }
}

settlement::TileImprovements HumanMind::GetProposedImprovement() 
{
    return settlement::TileImprovements(improvementTypeIndex);
}

settlement::Shipment currentShipment;

void HumanMind::ProcessTrade(Polity &polity) const
{
    static auto incomingShipments = container::Array <const settlement::Shipment *> (256);

    incomingShipments.Reset();

    for(auto &settlement : polity.GetSettlements())
    {
        auto &shipment = settlement->GetLastOutgoingShipment();

        if(shipment.TimeInAbsoluteTicks == WorldScene::Get()->GetTime().GetTickCount())
        {
            currentShipment = shipment;

            OnTradeShipment.Invoke();
        }

        for(auto &link : settlement->GetLinks())
        {
            if(link.Other->GetPolity() == &polity)
                continue;

            auto &incomingShipment = link.Other->GetLastOutgoingShipment();

            if(incomingShipment.TimeInAbsoluteTicks != WorldScene::Get()->GetTime().GetTickCount())
                continue;

            if(incomingShipment.To != settlement)
                continue;

            if(incomingShipments.Find(&incomingShipment) != nullptr)
                continue;

            currentShipment = incomingShipment;

            OnTradeShipment.Invoke();

            *incomingShipments.Add() = &incomingShipment;
        }
    }
}

void HumanMind::DecideBorders(Polity &polity) const
{
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

const tile::WorldTile *HumanMind::GetSettleTarget(settlement::Settlement *settlement) const
{
    auto target = settleTargets.Find(settlement);
    if(target == nullptr)
        return nullptr;
    else
        return target->Tile;
}

void HumanMind::RegisterPopIncrease(settlement::Settlement *settlement) const
{
    if(WorldScene::Get()->GetPlayerSettlement() != settlement)
        return;

    OnPlayerSettlementPopIncrease.Invoke();
}

tile::WorldTile *lastExploredTile = nullptr;

settlement::Settlement *lastExplorerSettlement = nullptr;

void HumanMind::RegisterTileExplored(settlement::Settlement *settlement, tile::WorldTile *tile) const
{
    lastExploredTile = tile;

    lastExplorerSettlement = settlement;

    interface::popup::PopupManager::Get()->AddPopup(PopupTypes::EXPLORATION_REWARD);
    //OnPlayerSettlementTileExplored.Invoke();
}

void HumanMind::RegisterProductionFinished(settlement::Settlement *settlement) const 
{
    if(settlement->GetCurrentProduction()->GetType() == settlement::ProductionOptions::SETTLERS)
    {
        auto target = settleTargets.Find(settlement);
        if(target != nullptr)
        {
            if(settlement->CanSettleHere(target->Tile) == true)
            {
                WorldScene::Get()->FoundSettlement(target->Tile, settlement->GetRace()->Type, settlement);

                OnPlayerSettlementColonized.Invoke();

                settlement->RemoveSettlers();
            }

            settleTargets.RemoveAt(target);
        }
    }
}

void HumanMind::RegisterMarkForDeletion() const 
{
    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    playerGroup->SetDomain(nullptr);

    OnPlayerPolityDeleted.Invoke();
}

tile::WorldTile *HumanMind::GetLastExploredTile()
{
    return lastExploredTile;
}

settlement::Settlement *HumanMind::GetLastExplorerSettlement()
{
    return lastExplorerSettlement;
}

void HumanMind::ProcessWorldUpdateData()
{
    const auto &explorationRewardDatas = WorldUpdateHandler::Get()->GetExploreRewardDatas();

    for(auto &data : explorationRewardDatas)
    {
        if(data.Settlement->GetPolity()->IsPlayerControlled() == false)
            continue;

        if(data.Reward.Type == settlement::ExplorationRewards::POPULATION)
        {
            RegisterPopIncrease(data.Settlement);
        }

        RegisterTileExplored(data.Settlement, data.Tile);
    }
}