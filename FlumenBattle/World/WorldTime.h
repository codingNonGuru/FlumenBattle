#pragma once

#include "FlumenCore/Utility/Utility.hpp"
#include "FlumenCore/Container/Array.hpp"

namespace world
{
    struct WorldTime
    {
        static constexpr auto HOUR_SIZE = 6;

        static constexpr auto DAYS_IN_YEAR = 365;

        static constexpr auto HOURS_IN_DAY = 24;

        static constexpr auto MINUTES_IN_DAYS = HOURS_IN_DAY * 60;

        static constexpr auto MINUTES_IN_YEAR = DAYS_IN_YEAR * MINUTES_IN_DAYS;

        static constexpr auto MINUTES_PER_TICK = 10;

        static constexpr auto TICKS_PER_HOUR = 6;

        static constexpr int MONTH_LENGTHS[12] = {30, 31, 30, 31, 30, 31, 30, 30, 31, 30, 31, 30};

        static constexpr const char *MONTH_LITERALS[12] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"};

        int MinuteCount;

        int TotalMinuteCount;

        int HourCount;

        int TotalHourCount;

        int DayCount;

        int TotalDayCount;

        int YearCount;

        int TickCount;

        bool IsFlowing;

        int FlowSpeed;

        bool IsStopDelayed;

        int StopDelay;

        bool IsNewHour;

        bool IsNewDay;

        bool IsNewYear;

        WorldTime() {}

        WorldTime(const WorldTime &) = delete;

        WorldTime &operator= (const WorldTime &) = delete;

        void Initialize(int yearCount, int dayCount, int hourCount) 
        {
            YearCount = yearCount;

            DayCount = dayCount;
            TotalDayCount = dayCount;

            HourCount = hourCount;
            TotalHourCount = hourCount;

            MinuteCount = 0;
            TotalMinuteCount = dayCount * MINUTES_IN_DAYS;

            IsFlowing = false;
            FlowSpeed = 1;
            IsStopDelayed = false;

            TickCount = TotalMinuteCount / MINUTES_PER_TICK;
        }

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
                case 4: return 0.0166f;
                case 3: return 0.0333f;
                case 2: return 0.2f;
                case 1: return 1.0f;
            }
        }

        WorldTime& operator++(int)
        {
            IsNewHour = false;
            IsNewDay = false;
            IsNewYear = false;

            TickCount++;

            MinuteCount += MINUTES_PER_TICK;
            TotalMinuteCount += MINUTES_PER_TICK;
            if(MinuteCount >= MINUTES_PER_TICK * HOUR_SIZE)
            {
                MinuteCount = 0;
                IsNewHour = true;

                HourCount++;
                TotalHourCount++;
                if(HourCount >= HOURS_IN_DAY)
                {
                    HourCount = 0;
                    IsNewDay = true;

                    DayCount++;
                    TotalDayCount++;
                    if(DayCount >= DAYS_IN_YEAR)
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

        int GetMonth()
        {
            auto index = 0;
            auto days = DayCount;
            while(true)
            {
                if(days < MONTH_LENGTHS[index])
                {
                    return index + 1;
                }
                else
                {
                    days -= MONTH_LENGTHS[index];
                }

                index++;
            }
        }

        const char *GetMonthName()
        {
            return MONTH_LITERALS[GetMonth() - 1];
        }

        int GetDayOfMonth()
        {
            auto index = 0;
            auto days = DayCount;
            while(true)
            {
                if(days < MONTH_LENGTHS[index])
                {
                    return days + 1;
                }
                else
                {
                    days -= MONTH_LENGTHS[index];
                }

                index++;
            }
        }

        int GetTickCount() const
        {
            return TickCount;
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