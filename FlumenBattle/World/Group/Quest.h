#pragma once

#include "FlumenBattle/World/WorldScene.h"

namespace world::settlement
{
    class Settlement;
}

namespace world::group
{
    class GroupCore;

    enum class QuestTypes
    {
        DEFEAT_ENEMY, DELIVER_ITEM
    };

    struct Quest
    {
        QuestTypes Type;

        settlement::Settlement *Origin;

        int DayCount;

        union QuestData
        {
            settlement::Settlement *TargetSettlement;

            GroupCore *TargetGroup;
        } Data;

        int TimeStamp = [] {return WorldScene::Get()->GetTime().TotalHourCount;} ();

        int GetDaysLeft() const;
    };
}