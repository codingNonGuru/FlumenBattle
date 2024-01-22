#pragma once

#include "FlumenBattle/Types.hpp"

namespace utility
{
    enum class RollDies
    {
        D4, D6, D8, D10, D12, D20, D100
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