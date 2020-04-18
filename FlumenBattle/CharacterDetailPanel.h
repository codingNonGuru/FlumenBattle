#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

class CharacterDetailPanel : public Element
{
    Text *strengthLabel;

    Text *dexterityLabel;

    Text *constitutionLabel;

    void HandleConfigure() override;

    void HandleUpdate() override;
};