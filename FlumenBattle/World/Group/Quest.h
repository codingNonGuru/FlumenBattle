#pragma once

namespace world::settlement
{
    class Settlement;
}

namespace world::group
{
    class Group;

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

            Group *TargetGroup;
        } Data;
    };
}