#pragma once

#include "FlumenCore/Utility/Utility.hpp"

namespace world
{
    struct WorldTime
    {
        int MinuteCount;

        int HourCount;

        int TotalHourCount;

        int DayCount;

        int TotalDayCount;

        int YearCount;

        bool IsFlowing;

        int FlowSpeed;

        bool IsStopDelayed;

        int StopDelay;

        bool IsNewHour;

        bool IsNewDay;

        bool IsNewYear;

        WorldTime() {}

        WorldTime(int yearCount, int dayCount, int hourCount) : 
            YearCount(yearCount), DayCount(dayCount), TotalDayCount(dayCount), HourCount(hourCount), TotalHourCount(hourCount), MinuteCount(0), IsFlowing(false), FlowSpeed(1), IsStopDelayed(false) {}

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
                case 5: return 0.001f;
                case 4: return 0.1f;
                case 3: return 0.2f;
                case 2: return 0.5f;
                case 1: return 1.0f;
            }
        }

        WorldTime& operator++(int)
        {
            IsNewHour = false;
            IsNewDay = false;
            IsNewYear = false;

            MinuteCount += 10;
            if(MinuteCount >= 60)
            {
                MinuteCount = 0;
                IsNewHour = true;

                HourCount++;
                TotalHourCount++;
                if(HourCount >= 24)
                {
                    HourCount = 0;
                    IsNewDay = true;

                    DayCount++;
                    TotalDayCount++;
                    if(DayCount >= 365)
                    {
                        DayCount = 0;
                        IsNewYear = true;
                        YearCount++;
                    }
                }
            }

            if(IsStopDelayed == true)
            {
                StopDelay--;
                if(StopDelay == 0)
                {
                    StopTime();

                    IsStopDelayed = false;
                }
            }

            return *this;
        }

        void StartTime(int stopDelay = 0)
        {
            IsFlowing = true;

            if(stopDelay != 0)
            {
                IsStopDelayed = true;
                StopDelay = stopDelay;
            }
        }

        void StopTime(int stopDelay = 0)
        {
            if(stopDelay == 0)
            {
                IsFlowing = false;
            }
            else
            {
                IsStopDelayed = true;
                StopDelay = stopDelay;
            }
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