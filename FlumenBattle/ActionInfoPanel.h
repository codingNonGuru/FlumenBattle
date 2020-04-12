#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

class ActionInfoPanel : public Element
{
    Array <Text*> actionLabels;

    Array <Text*> subactionLabels;

    bool wasLayoutRefreshed;

    LayoutGroup * actionLayoutGroup;

    LayoutGroup * subactionLayoutGroup;

    const Float LABEL_DISTANCE = 20.0f;

    void HandleConfigure() override;

    void HandleUpdate() override;

    void HandleCharacterSelected();

    void RefreshWeapons();

    void RefreshSpells();

    void HandleActionSelected();

    void HandleSubactionSelected();

    void HandleWeaponSelected();

    void HandleSpellSelected();

    void RefreshLayout();
};