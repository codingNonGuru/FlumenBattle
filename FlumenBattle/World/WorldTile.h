#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenBattle/World/Types.h"

namespace world
{
    class WorldMap;
    struct WorldBiome;
    struct WorldRelief;

    namespace group
    {
        class Group;
    }

    namespace settlement
    {
        class Settlement;
        struct PathSegment;
    }
}

namespace world
{
    struct WorldTile
    {
        friend class world::WorldMap;

        friend class world::group::Group;

        friend class WorldGenerator;

    private:
        WorldMap *map;

        group::Group *group;

        settlement::Settlement *settlement;

        settlement::Settlement *owner;

        Integer fertility;

        Integer industry;

        bool isBorderingOwnedTile;

        container::SmartBlock <settlement::PathSegment *, 6> links;

        int pathData;

    public:
        struct PathData
        {
            bool IsVisited;

            bool IsFreshlyVisited;

            bool IsFringe;

            Integer Passes[2];
        } PathData;

        Position2 Position;

        Integer2 SquareCoordinates;

        Integer3 HexCoordinates;

        Float4 Shade;

        WorldTiles Type;

        const WorldRelief *Relief;

        const WorldBiome *Biome;

        WorldTile() {}

        WorldTile(Position2 position, Integer2 squareCoordinates);

        void Initialize();

        Integer GetDistanceTo(const WorldTile& other) 
        {
            auto direction = this->HexCoordinates - other.HexCoordinates;
            return (abs(direction.x) + abs(direction.y) + abs(direction.z)) / 2;
        }

        Float GetPhysicalDistanceTo(const WorldTile& other)
        {
            auto direction = this->Position - other.Position;
            return glm::length(direction);
        }

        const Array <WorldTile*> & GetNearbyTiles(Integer, int = 0);

        const Array <WorldTile*> & GetTileRing(Integer);

        WorldTile * GetEmptyTileInRange(Integer);

        WorldTile * GetNeighbor(Integer3);

        group::Group * GetGroup() {return group;}

        settlement::Settlement * GetSettlement() {return settlement;}

        settlement::Settlement * GetOwner() {return owner;}

        Integer GetFertility() const {return fertility;}

        Integer GetIndustry() const {return industry;}

        Float4 GetShade() const;

        Integer GetTravelPenalty() const;

        void Settle(settlement::Settlement *);

        void AssertOwnership(settlement::Settlement *);

        bool IsOwned() const {return owner != nullptr;}

        settlement::Settlement * GetOwner() const {return owner;}

        bool IsBorderingOwnedTile() const {return isBorderingOwnedTile;}

        bool HasBiome(WorldBiomes biome) const;

        bool HasRelief(WorldReliefs relief) const;

        bool IsBlocked() const {return false;}

        bool IsLinkedTo(WorldTile *);

        void AddLink(WorldTile *, settlement::PathSegment *);

        settlement::PathSegment * GetLinkTo(WorldTile *);

        void SetPathData(int data) {pathData = data;}

        int GetPathData() {return pathData;}
    };
}