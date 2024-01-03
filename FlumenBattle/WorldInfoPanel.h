#pragma once

#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenCore/Singleton.h"

class Text;

namespace world::character
{
    class Character;
}

class WorldInfoPanel : public Element, public core::Singleton<WorldInfoPanel>
{
    friend class CharacterItem;

    class CharacterItem : public Element
    {
        world::character::Character *character;

        Text *classLabel;

        Text *healthLabel;

        Text *conditionsLabel;

        void HandleConfigure() override;    

        void HandleUpdate() override;    

    public:
        void * operator new(size_t size);

        void SetCharacter(world::character::Character *_character) {character = _character;}
    };

    Text *timeLabel;

    Text *speedLabel;

    Array <CharacterItem> items;

    void HandleConfigure() override;

    void HandleEnable() override;

    void HandleUpdate() override;

    Array <CharacterItem> & GetItemAllocator() {return items;}
};