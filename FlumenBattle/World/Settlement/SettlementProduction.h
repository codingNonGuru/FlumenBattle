#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Settlement/Types.h"

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

        SettlementProductionType(ProductionOptions type, Integer cost, void (*onFinish) (Settlement &)) : Type(type), Cost(cost), OnFinish(onFinish) {}

        SettlementProductionType(ProductionOptions type, Word name, Integer cost, void (*onFinish) (Settlement &)) : Type(type), Name(name), Cost(cost), OnFinish(onFinish) {}

        SettlementProductionType(ProductionOptions type, Word name, Integer cost, void (*onFinish) (Settlement &), void (*onInitialize) (Settlement &)) : 
            Type(type), Name(name), Cost(cost), OnFinish(onFinish), OnInitialize(onInitialize) {}

        ProductionOptions Type;

        Word Name;

        Integer Cost;

        void (*OnFinish) (Settlement &);

        void (*OnInitialize) (Settlement &);

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

    struct ProductionInquiry
    {
        bool CanProduce;

        ProductionData Data;
    };

    class SettlementProduction
    {
        friend class ProductionFinisher;

        const SettlementProductionType* type;

        Integer progress;

        ProductionData data;

    public:
        SettlementProduction() {}

        SettlementProduction(const SettlementProductionType *type, Integer progress, ProductionData data) : type(type), progress(progress), data(data) {}

        bool IsDone() const {return progress >= GetCost();}

        bool Is(ProductionOptions option) const {return GetType() == option;}

        void AddProgress(Integer progress) {this->progress += progress;}

        float GetProgressRatio() const {return float(this->progress) / float(GetCost());}

        int GetProgress() const {return this->progress;}

        int GetRemainingProgress() const {return type->Cost - this->progress;}

        Integer GetCost() const {return type->Cost;}

        ProductionOptions GetType() const {return type->Type;}

        Word GetName() const {return type->Name;}

        void Finish(Settlement &settlement) 
        {
            progress = 0;

            if(type->OnFinish != nullptr)
            {
                type->OnFinish(settlement);
            }
        }

        static int GetNecessity(Settlement &, ProductionOptions);

        static ProductionInquiry CanProduce(Settlement &, ProductionOptions);

        static const Word &GetName(ProductionOptions);

        static const SettlementProductionType *GetType(ProductionOptions);
    };

    class SettlementProductionFactory : public core::Singleton <SettlementProductionFactory>
    {
        friend class SettlementProduction;

        const SettlementProductionType *BuildProductionType(ProductionOptions);

        const SettlementProductionType *BuildPatrolProduction();

        const SettlementProductionType *BuildGarrisonProduction();

        const SettlementProductionType *BuildFarmProduction();

        const SettlementProductionType *BuildSewageProduction();

        const SettlementProductionType *BuildSettlersProduction();

        const SettlementProductionType *BuildIrrigationProduction();

        const SettlementProductionType *BuildLibraryProduction();

        const SettlementProductionType *BuildHousingProduction();

        const SettlementProductionType *BuildLumberMillProduction();

        const SettlementProductionType *BuildCarpenterProduction();

        const SettlementProductionType *BuildBakeryProduction();

        const SettlementProductionType *BuildWallsProduction();

        const SettlementProductionType *BuildNoneProduction();

    public:
        SettlementProduction Create(ProductionOptions, ProductionData = ProductionData());
    };

    class ProductionFinisher
    {
        friend class SettlementProductionFactory;

        static void FinishPatrol(Settlement &);

        static void FinishGarrison(Settlement &);

        static void FinishSewage(Settlement &);       

        static void FinishSettlers(Settlement &);       

        static void FinishFarm(Settlement &);  

        static void FinishIrrigation(Settlement &);  

        static void FinishLibrary(Settlement &);  

        static void FinishLumberMill(Settlement &);

        static void FinishCarpenter(Settlement &);

        static void FinishBakery(Settlement &);

        static void FinishHousing(Settlement &);  

        static void FinishWalls(Settlement &);  
    };
}