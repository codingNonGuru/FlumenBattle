#pragma once

#include "FlumenCore/Conventions.hpp"
#include "FlumenCore/Container/Map.hpp"
#include "FlumenCore/Hex/Hex.h"

#include "FlumenEngine/Thread/HexBuffer.h"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Settlement/Types.h"

struct Race;

namespace world
{
    namespace group
    {
        class GroupCore;
    }

    namespace settlement
    {
        class Settlement;
        struct PathSegment;
        enum class ResourceTypes;
    }
}

namespace world::tile
{
    class WorldMap;
    struct WorldRelief;
    struct WorldBiome;
    class RuinHandler;

    struct WorldTile : public core::hex::PhysicalTile
    {
        friend class WorldMap;

        friend class world::group::GroupCore;

        friend class WorldGenerator;

    public:
        static constexpr int MAXIMUM_TILE_HEAT = 100;

    private:
        WorldMap *map;

        settlement::Settlement *settlement;

        settlement::Settlement *owner;

        typedef int ResourceAmount;

        container::Block <ResourceAmount, BASIC_RESOURCES_COUNT> resources {0};

        bool isBorderingOwnedTile;

        bool isRevealed;

        container::SmartBlock <settlement::PathSegment *, 6> links;

        RuinHandler *ruinHandler;

        //int pathData;

    public:
        struct PathData
        {
            bool IsVisited = false;

            bool IsTotallyVisited;

            bool IsToBeVisited = false;
        } PathData;

        Integer2 SquareCoordinates;

        Integer Elevation;

        Integer Heat;

        Float4 Shade;

        WorldTiles Type;

        bool IsCoastline;

        bool IsPeninsula;

        bool IsBay;

        bool IsPeak;

        const WorldRelief *Relief;

        const WorldBiome *Biome;

        bool IsScrubland {false};

        RaceTypes MajorRace {RaceTypes::NONE};

        RaceTypes MinorRace {RaceTypes::NONE};

        WorldTile() {}

        WorldTile(Position2 position, Integer2 squareCoordinates);

        void Initialize();

        /*Integer GetDistanceTo(const WorldTile& other) 
        {
            auto direction = this->Coordinates - other.Coordinates;
            return (abs(direction.x) + abs(direction.y) + abs(direction.z)) / 2;
        }*/

        /*Float GetPhysicalDistanceTo(const WorldTile& other)
        {
            auto direction = this->Position - other.Position;
            return glm::length(direction);
        }*/

        Float GetPhysicalAngle(const WorldTile& other)
        {
            auto direction = this->Position - other.Position;
            auto angle = atan2(direction.y, direction.x);
            return angle;//angle + angle > 0 ? angle : (TWO_PI + angle);
        }

        const engine::thread::HexBuffer <WorldTile> GetNearbyTiles(Integer);

        container::Block <WorldTile *, 6> GetNearbyTiles();

        const engine::thread::HexBuffer <WorldTile> GetTileRing(Integer);

        WorldTile * GetEmptyTileInRange(Integer);

        WorldTile * GetNeighbor(Integer3);

        settlement::Settlement * GetSettlement() {return settlement;}

        settlement::Settlement * GetOwner() {return owner;}

        bool IsArid() const;

        Float4 GetShade() const;

        Integer GetPenalty() const;

        TravelPenalty GetTravelPenalty() const;

        void Settle(settlement::Settlement *);

        void AssertOwnership(settlement::Settlement *);

        void RemoveSettlement();

        bool IsOwned() const {return owner != nullptr;}

        settlement::Settlement * GetOwner() const {return owner;}

        bool IsBorderingOwnedTile() const {return isBorderingOwnedTile;}

        bool HasBiome(WorldBiomes biome) const;

        bool HasRelief(WorldReliefs relief) const;

        bool IsBlocked() const {return false;}

        bool IsLinkedTo(WorldTile *);

        void AddLink(WorldTile *, settlement::PathSegment *);

        settlement::PathSegment * GetLinkTo(WorldTile *);

        const container::SmartBlock <settlement::PathSegment *, 6> &GetLinks() const {return links;}

        //void SetPathData(int data) {pathData = data;}

        //int GetPathData() {return pathData;}

        ResourceAmount GetResource(settlement::ResourceTypes type) {return *resources[(int)type];}

        const WorldMap *GetMap() const {return map;}

        void SetResource(settlement::ResourceTypes type, ResourceAmount amount) {*resources[(int)type] = amount;}

        int GetSeasonalTemperature() const;

        int GetActualTemperature() const;

        bool IsWinter() const;

        void RevealTile() {isRevealed = true;}

        bool IsRevealed() const {return isRevealed;}

        void AddRuin(settlement::Settlement *, bool);

        int GetRuinCount() const;

        static float GetSeasonalTemperatureSwing();

        static float GetDiurnalTemperatureSwing();
    };
}