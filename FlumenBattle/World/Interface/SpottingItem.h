#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::group
{
    struct GroupSpotting;
}

namespace world::interface
{
    class SpottingItem : public Element
    {
        Element *border;

        Text *nameLabel;

        const group::GroupSpotting *spotting {nullptr};

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleDisable() override {spotting = nullptr;}

    public:
        bool HasContent(const group::GroupSpotting *spotting) {return this->spotting == spotting;}

        void Setup(const group::GroupSpotting *newSpotting) {spotting = newSpotting;}
    };
}