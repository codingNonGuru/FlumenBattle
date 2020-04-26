#pragma once

#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Interface/Text.hpp"

#include "FlumenBattle/Types.hpp"

class CharacterAbilityLabel : public Text
{
    friend class CharacterDetailPanel;

    friend class ElementFactory;

    struct AbilityData
    {
        AbilityTypes Type;

        Word Prefix;
    };

    AbilityData abilityData;

    void HandleUpdate() override;

    void SetAbilityData(AbilityData _abilityData) {abilityData = _abilityData;}

    CharacterAbilityLabel(FontDescriptor font, Color color) : Text(font, color) {}
};

class CharacterDetailPanel : public Element
{
    Text *headerLabel;

    Array <CharacterAbilityLabel *> abilityLabels;

    void HandleConfigure() override;

    void HandleUpdate() override;
};