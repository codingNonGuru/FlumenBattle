#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

class WorldDecisionMenu : public Element
{
    Text *searchLabel;

    Text *restLabel;

    void HandleConfigure() override;
};