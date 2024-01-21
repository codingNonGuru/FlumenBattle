#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace battle
{
    class CharacterInfo;
}

namespace world::interface
{
    class ResourceCounter;
}

namespace battle::interface
{
    class CharacterHoverInfo : public Element
    {
        CharacterInfo *characterInfo;

        Element *border;

        LayoutGroup *layout;

        Text *nameLabel;

        Text *classLabel;

        LayoutGroup *counterGroup;

        world::interface::ResourceCounter *healthCounter;

        world::interface::ResourceCounter *armorCounter;

        world::interface::ResourceCounter *attackCounter;

        world::interface::ResourceCounter *spellCounter;

        Text *conditionLabel;

        Position2 characterPosition;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(CharacterInfo *);
    };
}