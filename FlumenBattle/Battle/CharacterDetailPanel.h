#pragma once

#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Interface/Text.hpp"

#include "FlumenBattle/World/Character/Types.h"

#define FRIEND_CLASS friend class ElementFactory;

namespace battle
{
    class CharacterAbilityLabel : public Text
    {
        friend class CharacterDetailPanel;

        struct AbilityData
        {
            world::character::AbilityTypes Type;

            Word Prefix;
        };

        AbilityData abilityData;

        void HandleUpdate() override;

        void SetAbilityData(AbilityData _abilityData) {abilityData = _abilityData;}

    public:
        CharacterAbilityLabel(FontDescriptor font, Color color) : Text(font, color) {}
    };

    class CharacterDetailPanel : public Element
    {
        Element *border;
        
        Text *headerLabel;

        Array <CharacterAbilityLabel *> abilityLabels;

        void HandleConfigure() override;

        void HandleUpdate() override;
    };
}