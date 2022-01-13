#pragma once

#include "FlumenCore/Utility/Utility.hpp"

namespace world
{
    struct WorldTime
    {
        int MinuteCount;

        int HourCount;

        int DayCount;

        int YearCount;

        bool IsFlowing;

        int FlowSpeed;

        WorldTime() {}

        WorldTime(int yearCount, int dayCount, int hourCount) : 
            YearCount(yearCount), DayCount(dayCount), HourCount(hourCount), MinuteCount(0), IsFlowing(false), FlowSpeed(1) {}

        void SpeedUp() 
        {
            FlowSpeed++;
            utility::Clamp(FlowSpeed, 1, 5);
        }

        void SlowDown() 
        {
            FlowSpeed--;
            utility::Clamp(FlowSpeed, 1, 5);
        }

        float GetStep() const 
        {
            switch(FlowSpeed)
            {
                case 5: return 0.05f;
                case 4: return 0.1f;
                case 3: return 0.2f;
                case 2: return 0.5f;
                case 1: return 1.0f;
            }
        }

        WorldTime& operator++()
        {
            MinuteCount += 10;
            if(MinuteCount >= 60)
            {
                MinuteCount = 0;

                HourCount++;
                if(HourCount >= 24)
                {
                    HourCount = 0;

                    DayCount++;
                    if(DayCount >= 365)
                    {
                        DayCount = 0;
                        YearCount++;
                    }
                }
            }

            return *this;
        }

        WorldTime& operator =(bool value)
        {
            IsFlowing = value;
            return *this;
        }

        operator bool() const
        {
            return IsFlowing;
        }

        operator int() const 
        {
            return FlowSpeed;
        }
    };
}