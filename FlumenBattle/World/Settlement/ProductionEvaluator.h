#pragma once

#include "FlumenBattle/World/Settlement/Types.h"

namespace world::settlement
{
    class Settlement;

    struct NecessityFactor
    {
        static constexpr auto MINIMUM_VALUE = 0;

        static constexpr auto MAXIMUM_VALUE = 10;

        int Value;

        operator int() {return Value;}

        NecessityFactor(const int &value) 
        {
            Value = value;

            if(Value > MAXIMUM_VALUE)
                Value = MAXIMUM_VALUE;
            else if(Value < MINIMUM_VALUE)
                Value = MINIMUM_VALUE;
        }
    };

    class ProductionEvaluator
    {
        static NecessityFactor GetPatrolNecessity(const Settlement &);       

        static NecessityFactor GetSewageNecessity(const Settlement &);       

        static NecessityFactor GetSettlersNecessity(const Settlement &);       

        static NecessityFactor GetFarmNecessity(const Settlement &);  

        static NecessityFactor GetIrrigationNecessity(const Settlement &);  

        static NecessityFactor GetLibraryNecessity(const Settlement &);  

        static NecessityFactor GetHousingNecessity(const Settlement &);  

        static NecessityFactor GetWallsNecessity(const Settlement &);  

    public:
        static NecessityFactor GetNecessity(const Settlement &, ProductionOptions);
    };
}