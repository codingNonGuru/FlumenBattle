#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenCore/Singleton.h"

#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Settlement/Types.h"

namespace world
{
    namespace tile {struct WorldTile;}
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

        SettlementProductionType(ProductionOptions type, Word name, ProductionClasses productionClass, Integer cost, void (*onFinish) (Settlement &)) : 
            Type(type), Name(name), Class(productionClass), Cost(cost), OnFinish(onFinish) {}

        SettlementProductionType(ProductionOptions type, Word name, ProductionClasses productionClass, Integer cost, void (*onFinish) (Settlement &), void (*onInitialize) (Settlement &)) : 
            Type(type), Name(name), Class(productionClass), Cost(cost), OnFinish(onFinish), OnInitialize(onInitialize) {}

        ProductionOptions Type;

        Word Name;

        ProductionClasses Class;

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

        tile::WorldTile * colonizationTarget;

        ProductionData() {}

        ProductionData(SettlementTile *target) : improvementTarget(target) {}

        ProductionData(tile::WorldTile *target) : colonizationTarget(target) {}
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

        const SettlementProductionType *BuildProductionType(ProductionOptions) const;

        const SettlementProductionType *BuildPatrolProduction() const;

        const SettlementProductionType *BuildGarrisonProduction() const;

        const SettlementProductionType *BuildFarmProduction() const;

        const SettlementProductionType *BuildSewageProduction() const;

        const SettlementProductionType *BuildSettlersProduction() const;

        const SettlementProductionType *BuildIrrigationProduction() const;

        const SettlementProductionType *BuildLibraryProduction() const;

        const SettlementProductionType *BuildHousingProduction() const;

        const SettlementProductionType *BuildLumberMillProduction() const;

        const SettlementProductionType *BuildCarpenterProduction() const;

        const SettlementProductionType *BuildBakeryProduction() const;

        const SettlementProductionType *BuildWallsProduction() const;

        const SettlementProductionType *BuildWeavingMillProduction() const;

        const SettlementProductionType *BuildTailoryProduction() const;

        const SettlementProductionType *BuildKeepProduction() const;

        const SettlementProductionType *BuildMarketProduction() const;

        const SettlementProductionType *BuildPotteryProduction() const;

        const SettlementProductionType *BuildNoneProduction() const;

    public:
        SettlementProduction Create(ProductionOptions, ProductionData = ProductionData());

        const container::Array <const SettlementProductionType *> &GetRecruitmentOptions() const;

        const container::Array <const SettlementProductionType *> &GetBuildingOptions() const;
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

        static void FinishMarket(Settlement &);

        static void FinishHousing(Settlement &);  

        static void FinishWalls(Settlement &);  

        static void FinishWeavingMill(Settlement &);

        static void FinishTailory(Settlement &);

        static void FinishKeep(Settlement &);

        static void FinishPottery(Settlement &);
    };
}