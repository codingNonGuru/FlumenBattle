#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

class WorldDecisionMenu : public Element
{
    Text *mainLabel;

    void HandleConfigure() override;
};