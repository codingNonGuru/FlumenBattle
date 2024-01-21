#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace battle
{
    class BattleEndMessage : public Element
    {
        Text *mainLabel;

        void HandleConfigure() override;

        void HandleEnable() override;

        void HandleConfirm();
    };
}