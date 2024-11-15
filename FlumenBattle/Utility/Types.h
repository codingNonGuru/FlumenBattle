#pragma once

#include "FlumenBattle/Types.hpp"

namespace utility
{
    enum class RollDies
    {
        D4 = 4, D6 = 6, D8 = 8, D10 = 10, D12 = 12, D20 = 20, D100 = 100
    };

    enum class SuccessTypes
    {
        CRITICAL_SUCCESS, SUCCESS, FAILURE, CRITICAL_FAILURE
    };

    template<typename T>
	concept CanBeTravelled = requires(T a, int b)
	{
        { a.GetNearbyTiles(b) };

        { a.GetTravelPenalty() } -> std::same_as <TravelPenalty>;
	};
}