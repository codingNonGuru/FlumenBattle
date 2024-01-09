#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Types.h"

namespace world
{
    class WorldTile;
}

namespace world::settlement
{
    class SettlementProductionFactory;
    class Settlement;
    struct SettlementTile;
    class SettlementProduction;

    struct SettlementProductionType
    {
        friend class SettlementProduction;

        friend class SettlementProductionFactory;

    private:
        SettlementProductionType(SettlementProductionOptions type, Integer cost, void (*onFinish) (Settlement &)) : Type(type), Cost(cost), OnFinish(onFinish) {}

        SettlementProductionType(SettlementProductionOptions type, Word name, Integer cost, void (*onFinish) (Settlement &)) : Type(type), Name(name), Cost(cost), OnFinish(onFinish) {}

        SettlementProductionOptions Type;

        Word Name;

        Integer Cost;

        void (*OnFinish) (Settlement &);

        SettlementProductionType(const SettlementProductionType &) = delete;

        SettlementProductionType& operator=(const SettlementProductionType &) = delete;
    };

    union ProductionData
    {
    public:
        SettlementTile * improvementTarget;

        world::WorldTile * colonizationTarget;

        ProductionData() {}

        ProductionData(SettlementTile *target) : improvementTarget(target) {}

        ProductionData(world::WorldTile *target) : colonizationTarget(target) {}
    };

    class SettlementProduction
    {
        friend class SettlementProductionFinisher;

        const SettlementProductionType* type;

        Integer progress;

        ProductionData data;

    public:
        SettlementProduction() {}

        SettlementProduction(const SettlementProductionType *type, Integer progress, ProductionData data) : type(type), progress(progress), data(data) {}

        bool IsDone() const {return progress >= GetCost();}

        bool Is(SettlementProductionOptions option) const {return GetType() == option;}

        void AddProgress(Integer progress) {this->progress += progress;}

        float GetProgressRatio() const {return float(this->progress) / float(GetCost());}

        Integer GetCost() const {return type->Cost;}

        SettlementProductionOptions GetType() const {return type->Type;}

        Word GetName() const {return type->Name;}

        void Finish(Settlement &settlement) 
        {
            progress = 0;

            if(type->OnFinish != nullptr)
            {
                type->OnFinish(settlement);
            }
        }
    };

    class SettlementProductionFactory : public core::Singleton <SettlementProductionFactory>
    {
        const SettlementProductionType *BuildProductionType(SettlementProductionOptions);

        const SettlementProductionType *BuildPatrolProduction();

        const SettlementProductionType *BuildFarmProduction();

        const SettlementProductionType *BuildSewageProduction();

        const SettlementProductionType *BuildSettlersProduction();

        const SettlementProductionType *BuildIrrigationProduction();

        const SettlementProductionType *BuildLibraryProduction();

        const SettlementProductionType *BuildNoneProduction();

    public:
        SettlementProduction Create(SettlementProductionOptions, ProductionData = ProductionData());
    };

    class SettlementProductionFinisher
    {
        friend class SettlementProductionFactory;

        static void FinishPatrol(Settlement &);       

        static void FinishSewage(Settlement &);       

        static void FinishSettlers(Settlement &);       

        static void FinishFarm(Settlement &);  

        static void FinishIrrigation(Settlement &);  

        static void FinishLibrary(Settlement &);  
    };
}