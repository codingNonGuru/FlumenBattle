#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::group
{
    struct GroupSpotting;
}

namespace world::interface
{
    class ResourceCounter;

    class SpottingItem : public Element
    {
        Element *border;

        Text *nameLabel;

        ResourceCounter *timeCounter;

        int time;

        Element *skullIcon;

        const group::GroupSpotting *spotting {nullptr};

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleDisable() override {spotting = nullptr;}

        void HandleLeftClick() override;

    public:
        bool HasContent() const {return spotting != nullptr;}

        bool HasContent(const group::GroupSpotting &);

        void Setup(const group::GroupSpotting *newSpotting) {spotting = newSpotting;}
    };
}