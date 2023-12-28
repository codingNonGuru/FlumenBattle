#pragma once

#include "FlumenCore/Conventions.hpp"

#include "FlumenEngine/Core/Singleton.h"

#include "FlumenBattle/Utility/Types.h"

namespace utility
{
    typedef int Result;

    Result RollD4Dice();

    Result RollD20Dice();

    typedef int Bonus;

    typedef int DifficultyClass;

    struct Success
    {
        SuccessTypes Type;

        Result Roll;

        Bonus Modifier;

        DifficultyClass DC;

    public:
        Success() {}

        Success(const SuccessTypes &type) : Type(type) {}

        Success(SuccessTypes type, Result roll, Bonus modifier, DifficultyClass dC) : 
        Type(type), Roll(roll), Modifier(modifier), DC(dC) {}

        bool IsAnyFailure() const {return Type == SuccessTypes::FAILURE || Type == SuccessTypes::CRITICAL_FAILURE;}

        bool IsAnySuccess() const {return Type == SuccessTypes::SUCCESS || Type == SuccessTypes::CRITICAL_SUCCESS;}

        bool IsNormalSuccess() const {return Type == SuccessTypes::SUCCESS;}

        bool IsCriticalSuccess() const {return Type == SuccessTypes::CRITICAL_SUCCESS;}

        //bool operator ==(const Success &other) {return other.Type == Type;}

        bool operator ==(const SuccessTypes &other) const {return other == Type;}

        operator SuccessTypes() {return Type;}
    };

    Success RollD20Dice(DifficultyClass, Bonus = 0);

    Result RollD100Dice();

    template<typename T>
	concept CanBeTravelled = requires(T a, int b)
	{
        { a.GetNearbyTiles(b) };

        { a.IsBlocked() };
	};

    template <class TileType> requires CanBeTravelled <TileType>
    class Pathfinder : public Singleton <Pathfinder <TileType>>
    {
        struct TileData
        {
            TileType *Tile;

            Integer Distance;

            TileData(TileType *tile, Integer distance) : Tile(tile), Distance(distance) {}
        };

        Array <TileData> visitedTiles;

        Array <Array <TileType *>> fringeTiles;

    public:
        Pathfinder()
        {
            visitedTiles = Array <TileData>(1024);

            fringeTiles = [] () {
                static auto arrays = Array <Array <TileType *>>(128);

                for(int i = 0; i < arrays.GetCapacity(); ++i)
                {
                    *arrays.Add() = Array <TileType *> (128);
                }

                return arrays;
            } ();
        }

        const Array <TileData> & FindPath(TileType *startingTile, Integer range)
        {
            visitedTiles.Reset();
            *visitedTiles.Add() = TileData(startingTile, 0);

            fringeTiles.Reset();
            
            auto tileArray = fringeTiles.Add();
            tileArray->Reset();
            *tileArray->Add() = startingTile;

            for(int k = 1; k <= range; k++)
            {
                fringeTiles.Add()->Reset();
                for(auto &tile : *fringeTiles.Get(k - 1))
                {
                    auto neighbours = tile->GetNearbyTiles(1);
                    for(auto neighbour : neighbours)
                    {
                        //if(neighbour->IsObstacle || neighbour->Combatant)
                        if(neighbour->IsBlocked() == true)
                            continue;

                        bool hasFound = false;
                        for(auto &visitedTile : visitedTiles)
                        {
                            if(visitedTile.Tile == neighbour)
                            {
                                hasFound = true;
                                break;
                            }
                        }

                        if(hasFound == true)
                            continue;

                        *visitedTiles.Add() = {neighbour, k};
                        *fringeTiles.Get(k)->Add() = neighbour;
                    }
                }
            }

            return visitedTiles;
        }
    };
}