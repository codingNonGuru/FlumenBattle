#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::character
{
    class Character;
}

namespace world::interface
{
    class InventoryMenu : public Element
    {
        Text *characterLabel;

        character::Character *character;

        void HandleConfigure() override;

    public:
        void SelectCharacter(character::Character *);
    };
}