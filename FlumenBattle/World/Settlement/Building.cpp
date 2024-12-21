#include "Building.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/SettlementTile.h"
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

    class LumberMill : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            
        }
    };

    class Carpenter : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            
        }
    };

    class Bakery : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            
        }
    };

    class WeavingMill : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            
        }
    };

    class Tailory : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            
        }
    };

    class Walls : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            auto wallsLevel = settlement.GetBuildingCount(BuildingTypes::WALLS);

            settlement.AddModifier({Modifiers::DEFENDER_GROUP_BONUS_AC, wallsLevel});
        }
    };

    class Keep : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            settlement.AddModifier({Modifiers::LOOT_DC_BONUS, 5});
            settlement.AddModifier({Modifiers::PILLAGE_DC_BONUS, 5});

            settlement.AddModifier({Modifiers::DEFENDER_GROUP_BONUS_INITIATIVE, 2});
        }
    };

    class Market : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
            auto count = settlement.GetBuildingCount(BuildingTypes::MARKET);

            settlement.AddModifier({Modifiers::DURATION_BETWEEN_TRADE_SHIPMENTS, -count * 2});
        }
    };

    class Pottery : public BuildingType
    {
        using BuildingType::BuildingType; 

        void HandleApplyEffect(Settlement &settlement) const override
        {
        }
    };
}

void Building::ApplyEffect(Settlement &settlement) const
{
    if(amount == 0)
        return;
        
    type->HandleApplyEffect(settlement);
}

void Building::Destroy() 
{
    amount--;

    if(amount == 0)
    {
        
    }
}

Building BuildingFactory::Create(BuildingTypes type)
{
    switch(type)
    {
    case BuildingTypes::WALLS:
        return 
        {
            [&] {static const auto buildingType = Walls(type, 300, true, "Walls", "Walls"); return &buildingType;} ()
        };
    case BuildingTypes::LIBRARY:
        return 
        {
            [&] {static const auto buildingType = Library(type, 200, true, "Library", "Library"); return &buildingType;} ()
        };
    case BuildingTypes::SEWAGE:
        return 
        {
            [&] {static const auto buildingType = Sewage(type, 200, false, "Sewage", "Sewage"); return &buildingType;} ()
        };
    case BuildingTypes::IRRIGATION:
        return 
        {
            [&] {static const auto buildingType = Irrigation(type, 200, false, "Irrigation", "Irrigation"); return &buildingType;} ()
        };
    case BuildingTypes::HOUSING:
        return 
        {
            [&] {static const auto buildingType = Housing(type, 200, true, "Housing", "Houses64"); return &buildingType;} ()
        };
    case BuildingTypes::LUMBER_MILL:
        return 
        {
            [&] {static const auto buildingType = LumberMill(type, 200, true, "Lumber mill", "LumberMill", {ResourceTypes::LUMBER, 1}, {{ResourceTypes::TIMBER, 3}}); return &buildingType;} ()
        };
    case BuildingTypes::CARPENTER:
        return 
        {
            [&] {static const auto buildingType = Carpenter(type, 200, true, "Carpenter", "LumberMill", {ResourceTypes::FURNITURE, 1}, {{ResourceTypes::LUMBER, 3}}); return &buildingType;} ()
        };
    case BuildingTypes::BAKERY:
        return 
        {
            [&] {static const auto buildingType = Bakery(type, 200, true, "Bakery", "LumberMill", {ResourceTypes::COOKED_FOOD, 2}, {{ResourceTypes::FOOD, 3}, {ResourceTypes::TIMBER, 1}}); return &buildingType;} ()
        };
    case BuildingTypes::MARKET:
        return 
        {
            [&] {static const auto buildingType = Market(type, 200, true, "Market", "Market"); return &buildingType;} ()
        };
    case BuildingTypes::WEAVING_MILL:
        return 
        {
            [&] {static const auto buildingType = WeavingMill(type, 200, true, "Weaving mill", "LumberMill", {ResourceTypes::FABRIC, 2}, {{ResourceTypes::FIBER, 3}}); return &buildingType;} ()
        };
    case BuildingTypes::TAILORY:
        return 
        {
            [&] {static const auto buildingType = Tailory(type, 200, true, "Tailory", "LumberMill", {ResourceTypes::CLOTHING, 2}, {{ResourceTypes::FABRIC, 3}}); return &buildingType;} ()
        };
    case BuildingTypes::KEEP:
        return 
        {
            [&] {static const auto buildingType = Keep(type, 200, true, "Keep", "Keep"); return &buildingType;} ()
        };
    case BuildingTypes::POTTERY:
        return 
        {
            [&] {static const auto buildingType = Pottery(type, 200, true, "Pottery", "LumberMill", {ResourceTypes::POTTERY, 2}, {{ResourceTypes::CLAY, 3}, {ResourceTypes::TIMBER, 4}}); return &buildingType;} ()
        };
    }
}

int Building::GetResourceConsumption(ResourceTypes resourceType) const
{
    for(auto &resource : type->InputResources)
    {
        if(resource.Resource == resourceType)
        {
            return resource.Amount;
        }
    }

    return 0;
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

int BuildingManager::GetBuildingCount() const
{
    auto count = 0;

    for(auto &building : buildingSet.buildings)
    {
        count += building.GetAmount();
    }

    return count;
}

int BuildingManager::GetStandingBuildingCount() const
{
    auto count = 0;

    for(auto &building : buildingSet.buildings)
    {
        count += building.GetAmount() - building.GetDamagedCount();
    }

    return count;
}

const container::Array <Building *> &BuildingManager::GetBuildingsThatProduce(ResourceTypes resource) const
{
    static auto buildings = container::Array <Building *> (32);

    buildings.Reset();

    for(auto &building : buildingSet.buildings)
    {
        if(building.DoesProduce(resource) == false)
            continue;

        *buildings.Allocate() = &building;
    }

    return buildings;
}

Building &BuildingManager::GetRandomStandingBuilding()
{
    while(true)
    {
        auto building = buildingSet.buildings.GetRandom();
        if(building->GetDamagedCount() < building->GetAmount())
        {
            return *building;
        }
    }
}

Building &BuildingManager::GetRandomBuilding()
{
    while(true)
    {
        auto building = buildingSet.buildings.GetRandom();
        if(building->GetAmount() != 0)
        {
            return *building;
        }
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

    buildingSet.buildings.RemoveAt(buildingPointer);
}

void BuildingManager::RemoveBuilding(Building &building)
{
    buildingSet.buildings.RemoveAt(&building);
}

void BuildingManager::ApplyModifiers(Settlement &settlement) const
{
    for(const auto &building : buildingSet.Get())
    {
        building.ApplyEffect(settlement);
    }
}

void BuildingManager::RemovePersonnel()
{
    for(auto &building : buildingSet.Get())
    {
        building.activePersonnelCount = 0;
    }
}

void BuildingManager::Update()
{
    for(auto &building : buildingSet.buildings)
    {
        if(building.GetAmount() == 0)
        {
            RemoveBuilding(building);
        }
    }
}

void BuildingDamager::DamageImprovements(const disaster::Earthquake &earthquake, Settlement &settlement)
{
    using namespace utility;

    auto difficultyClass = earthquake.GetDifficultyClass();
    
    auto settlementModifier = settlement.GetModifier(settlement::Modifiers::BUILDING_SAVING_THROWS_AGAINST_EARTHQUAKES);

    auto &tiles = settlement.tiles;
    for(auto &tile : tiles)
    {
        if(tile.IsImproved() == false)
        {
            continue;
        }

        auto success = utility::RollD20Dice(difficultyClass, settlementModifier);
        if(success.IsAnySuccess())
        {
            continue;
        }

        tile.ResetImprovement();
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