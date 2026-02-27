#include "Mind.h"
#include "WorkInstruction.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Job.h"
#include "FlumenBattle/Config.h"

using namespace world::polity;

void Mind::UpdateSettlementWorkforce(settlement::Settlement *settlement, const container::Pool <WorkInstruction> &instructions) const
{
    static const auto MAX_SETTLEMENT_POPULATION = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAX_SETTLEMENT_POPULATION).Integer;

    static container::Array <settlement::Job *> removalQueue(MAX_SETTLEMENT_POPULATION);

    removalQueue.Reset();

    auto &jobs = settlement->GetResourceHandler().GetJobs();

    for(auto &job : jobs)
    {
        auto instruction = instructions.Find(job.GetCohort());
        if(instruction == nullptr)
        {
            *removalQueue.Add() = &job;
        }
    }

    for(auto &job : removalQueue)
    {
        settlement->FireWorker(job);
    }

    for(auto &instruction : instructions)
    {
        auto job = jobs.Find(instruction.Cohort);
        if(job == nullptr)
        {
            if(instruction.PlaceType == WorkInstruction::RESOURCE)
            {
                settlement->HireWorker(instruction.Place.Resource->Type->Type, instruction.Cohort);
            }
            else if(instruction.PlaceType == WorkInstruction::TILE)
            {
                settlement->HireWorker(instruction.Place.Tile, instruction.Cohort);
            }   
        }
    }
}