#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

class BattleEndMessage : public Element
{
    Text *mainLabel;

    void HandleConfigure() override;

    void HandleEnable() override;

    void HandleConfirm();
};