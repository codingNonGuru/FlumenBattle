#include "Building.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Disaster/Earthquake.h"
#include "FlumenBattle/Utility/Utility.h"

using namespace world::settlement;

namespace world::settlement
{
    class Library : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            settlement.AddModifier({Modifiers::SCIENCE_PRODUCTION, 1});
        }
    };

    class Sewage : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            settlement.AddModifier({Modifiers::SAVING_THROWS_AGAINST_DISEASE, 1});
        }
    };

    class Irrigation : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            settlement.AddModifier({Modifiers::FOOD_PRODUCTION_ON_DESERT_TILES, 1});
        }
    };

    class Housing : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            
        }
    };
}

void Building::ApplyEffect(Settlement &settlement) const
{
    type->HandleApplyEffect(settlement);
}

Building BuildingFactory::Create(BuildingTypes type)
{
    switch(type)
    {
    case BuildingTypes::LIBRARY:
        return 
        {
            [&] {static const auto buildingType = Library(type, 200, true); return &buildingType;} (), 
            false
        };
    case BuildingTypes::SEWAGE:
        return 
        {
            [&] {static const auto buildingType = Sewage(type, 200, false); return &buildingType;} (), 
            false
        };
    case BuildingTypes::IRRIGATION:
        return 
        {
            [&] {static const auto buildingType = Irrigation(type, 200, false); return &buildingType;} (), 
            false
        };
    case BuildingTypes::HOUSING:
        return 
        {
            [&] {static const auto buildingType = Housing(type, 200, true); return &buildingType;} (), 
            false
        };
    }
}

void BuildingManager::Initialize(Settlement *settlement)
{
    this->settlement = settlement;
}

bool BuildingManager::HasBuilding(BuildingTypes type) const
{
    return GetBuildingCount(type) > 0;
}

int BuildingManager::GetBuildingCount(BuildingTypes type) const
{
    auto building = buildingSet.buildings.Find(type);
    if(building == nullptr)
    {
        return 0;
    }
    else
    {
        return building->GetAmount();
    }
}

void BuildingManager::AddBuilding(BuildingTypes type)
{
    auto building = buildingSet.buildings.Find(type);
    if(building == nullptr)
    {
        const auto &building = BuildingFactory::Get()->Create(type);

        auto buildingPointer = buildingSet.buildings.Add();

        *buildingPointer = building;
    }
    else
    {
        building->IncreaseAmount();
    }
}

void BuildingManager::RemoveBuilding(BuildingTypes type)
{
    auto buildingPointer = buildingSet.buildings.Find(type);

    buildingPointer->Destroy();

    //buildingSet.buildings.RemoveAt(buildingPointer);
}

void BuildingManager::ApplyModifiers(Settlement &settlement) const
{
    for(const auto &building : buildingSet.Get())
    {
        building.ApplyEffect(settlement);
    }
}

void BuildingManager::Update()
{

}

void BuildingDamager::DamageImprovements(const disaster::Earthquake &earthquake, Settlement &settlement)
{
    using namespace utility;

    auto difficultyClass = earthquake.GetDifficultyClass();
    
    auto settlementModifier = settlement.GetModifier(settlement::Modifiers::BUILDING_SAVING_THROWS_AGAINST_EARTHQUAKES);

    auto &tiles = settlement.tiles;
    for(auto &tile : tiles)
    {
        if(tile.IsBuilt == false)
        {
            continue;
        }

        auto success = utility::RollD20Dice(difficultyClass, settlementModifier);
        if(success.IsAnySuccess())
        {
            continue;
        }

        tile.IsBuilt = false;
    }
}

void BuildingDamager::DamageBuildings(const disaster::Earthquake &earthquake, BuildingManager &buildingManager) 
{
    using namespace utility;

    auto difficultyClass = earthquake.GetDifficultyClass();
    
    auto settlementModifier = buildingManager.settlement->GetModifier(settlement::Modifiers::BUILDING_SAVING_THROWS_AGAINST_EARTHQUAKES);

    auto &buildings = buildingManager.buildingSet.buildings;
    for(auto &building : buildings)
    {
        auto heightBonus = building.IsTall() ? 0 : 1;

        auto success = utility::RollD20Dice(difficultyClass, settlementModifier + heightBonus);
        if(success.IsAnySuccess())
        {
            continue;
        }

        buildings.RemoveAt(&building);
    }
}