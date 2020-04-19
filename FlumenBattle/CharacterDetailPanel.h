#pragma once

#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Interface/Text.hpp"

#include "FlumenBattle/Types.hpp"

class CharacterAbilityLabel : public Text
{
    friend class CharacterDetailPanel;

    AbilityTypes abilityType;

    Word abilityPrefix;

    CharacterAbilityLabel(Font *font, Color color, AbilityTypes type, Word prefix) : Text(font, color), abilityType(type), abilityPrefix(prefix) {}

    void HandleUpdate() override;
};

class CharacterDetailPanel : public Element
{
    Text *headerLabel;

    Array <CharacterAbilityLabel *> abilityLabels;

    void HandleConfigure() override;

    void HandleUpdate() override;
};