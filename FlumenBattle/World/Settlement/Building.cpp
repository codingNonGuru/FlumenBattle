#include "Building.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Disaster/Earthquake.h"

using namespace world::settlement;

namespace world::settlement
{
    class Library : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) override
        {
            settlement.AddModifier({Modifiers::SCIENCE_PRODUCTION, 1});
        }
    };

    class Sewage : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) override
        {
            settlement.AddModifier({Modifiers::SAVING_THROWS_AGAINST_DISEASE, 1});
        }
    };

    class Irrigation : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) override
        {
            settlement.AddModifier({Modifiers::FOOD_PRODUCTION_ON_DESERT_TILES, 1});
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
            [&] {static const auto buildingType = Library(type, 200); return &buildingType;} (), 
            false,
            true
        };
    case BuildingTypes::SEWAGE:
        return 
        {
            [&] {static const auto buildingType = Sewage(type, 200); return &buildingType;} (), 
            false,
            false
        };
    case BuildingTypes::IRRIGATION:
        return 
        {
            [&] {static const auto buildingType = Irrigation(type, 200); return &buildingType;} (), 
            false,
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
    return buildingSet.buildings.Find(type) != nullptr;
}

void BuildingManager::AddBuilding(BuildingTypes type)
{
    const auto &building = BuildingFactory::Get()->Create(type);

    auto buildingPointer = buildingSet.Get().Add();

    *buildingPointer = building;
}

void BuildingManager::RemoveBuilding(BuildingTypes type)
{
    auto &buildings = buildingSet.Get();

    auto buildingPointer = buildings.Find(type);

    buildings.Remove(buildingPointer);
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
    auto difficultyClass = earthquake.GetDifficultyClass();
    
    auto settlementModifier = settlement.GetModifier(settlement::Modifiers::BUILDING_SAVING_THROWS_AGAINST_EARTHQUAKES);

    auto &tiles = settlement.tiles;
    for(auto &tile : tiles)
    {
        if(tile.IsBuilt == false)
        {
            continue;
        }

        auto diceRoll = utility::GetRandom(1, 20) + settlementModifier;
        if(diceRoll >= difficultyClass)
        {
            continue;
        }

        tile.IsBuilt = false;
    }
}

void BuildingDamager::DamageBuildings(const disaster::Earthquake &earthquake, BuildingManager &buildingManager) 
{
    auto difficultyClass = earthquake.GetDifficultyClass();
    
    auto settlementModifier = buildingManager.settlement->GetModifier(settlement::Modifiers::BUILDING_SAVING_THROWS_AGAINST_EARTHQUAKES);

    auto &buildings = buildingManager.buildingSet.Get();
    for(auto &building : buildings)
    {
        auto heightBonus = building.IsTall() ? 0 : 1;

        auto diceRoll = utility::GetRandom(1, 20) + settlementModifier + heightBonus;
        if(diceRoll >= difficultyClass)
        {
            continue;
        }

        buildings.Remove(&building);
    }
}