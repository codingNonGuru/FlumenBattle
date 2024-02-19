#include "Quest.h"

using namespace world::group;

int Quest::GetDaysLeft() const
{
    static const auto &time = WorldScene::Get()->GetTime();

    auto timeElapsed = time.TotalHourCount - TimeStamp;
    auto daysElapsed = timeElapsed / WorldTime::HOURS_IN_DAY;
    
    return DayCount - daysElapsed;
}