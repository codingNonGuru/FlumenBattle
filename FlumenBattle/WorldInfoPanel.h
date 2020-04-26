#pragma once

#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenEngine/Core/Singleton.h"

class Text;

class Character;

class WorldInfoPanel : public Element, public Singleton<WorldInfoPanel>
{
    friend class CharacterItem;

    class CharacterItem : public Element
    {
        Character *character;

        Text *classLabel;

        Text *healthLabel;

        void HandleConfigure() override;    

        void HandleUpdate() override;    

    public:
        void * operator new(size_t size);

        void SetCharacter(Character *_character) {character = _character;}
    };

    Array <CharacterItem> items;

    void HandleConfigure() override;

    void HandleEnable() override;

    Array <CharacterItem> & GetItemAllocator() {return items;}
};