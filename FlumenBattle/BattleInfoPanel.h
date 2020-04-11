#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

class BattleInfoPanel : public Element
{
    Text* actionLabel;

    void HandleConfigure() override;

    void HandleUpdate() override;

    void HandleCharacterAction();
};