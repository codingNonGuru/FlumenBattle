#pragma once

#include "FlumenEngine/Interface/Element.hpp"
#include "FlumenCore/Singleton.h"

class Text;
class Animation;

namespace world::character
{
    class Character;
}

namespace world::interface
{
    class ResourceCounter;
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

        Element *icon;

        Element *border;

        Element *cover;

        Text *healthDifferenceLabel;

        bool isSelected {false};

        int previousHitpoints;

        Animation *animation;

        void HandleConfigure() override;    

        void HandleUpdate() override;  

    public:
        void * operator new(size_t size);

        void SetCharacter(world::character::Character *_character);

        void ToggleSelection();

        void ForceSelection();
    };

    Text *timeLabel;

    Text *speedLabel;

    world::interface::ResourceCounter *moneyCounter;

    world::interface::ResourceCounter *foodCounter;

    world::interface::ResourceCounter *perceptionCounter;

    world::interface::ResourceCounter *weightCounter;

    world::interface::ResourceCounter *muleCounter;

    Array <CharacterItem> items;

    int selectionIndex {-1};

    void HandleConfigure() override;

    void HandleEnable() override;

    void HandleUpdate() override;

    Array <CharacterItem> & GetItemAllocator() {return items;}

public:
    void SelectCharacter(int, bool);

    void HandleInventoryOpen(int);
};