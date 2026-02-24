#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class Text;
class LayoutGroup;

namespace world::settlement
{
    class Settlement;
}

namespace world::interface
{
    class ResourceCounter;
}

namespace world::interface::rule
{
    class RuleMenu;
    class DomainHoverInfo;
    class RealmTab;
    class DomainItem;

    class RealmTab : public Element
    {
        LayoutGroup *domainLayout;

        container::Array <DomainItem *> domainItems;

        DomainHoverInfo *hoverInfo;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        DomainHoverInfo *GetHoverInfo() {return hoverInfo;}
    };
}