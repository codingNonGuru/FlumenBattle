#pragma once

namespace world
{
    class WorldTile;
}

namespace world::settlement
{
    struct TileImprovementType;
    enum class ResourceTypes;
    enum class TileImprovements;

    struct SettlementTile
    {
    private:
        const TileImprovementType *ImprovementType;

    public:
        world::WorldTile *Tile;

        bool IsWorked;

        bool IsImproved() const {return ImprovementType != nullptr;}

        void ResetImprovement() {ImprovementType = nullptr;}

        int GetImprovementBonus(ResourceTypes) const;

        const TileImprovementType *GetImprovementType() const {return ImprovementType;}
        
        void AddImprovement(TileImprovements);

        bool HasImprovement(TileImprovements) const;

        bool operator==(const world::WorldTile *otherTile) {return Tile == otherTile;}
    };
}