#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Character;
class Text;

class CharacterInfo : public Element
{
    Character* character;

    Text* textLabel;

    void HandleConfigure() override;

    friend class CharacterModel;
};