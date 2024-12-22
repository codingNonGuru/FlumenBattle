#pragma once

#include "FlumenBattle/World/Interface/Popup/GenericPopup.h"

namespace world::interface::popup
{
    class ExplorationReward : public GenericPopup
    {
    public:
        void Setup() override;
    };
}