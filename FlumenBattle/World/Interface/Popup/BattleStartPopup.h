#pragma once

#include "FlumenBattle/World/Interface/Popup/GenericPopup.h"

namespace world::interface::popup
{
    class BattleStartPopup : public GenericPopup
    {
    public:
        void Setup() override;
    };
}