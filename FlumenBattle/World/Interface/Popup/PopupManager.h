#pragma once

#include "FlumenCore/Singleton.h"
#include "FlumenCore/Container/Pool.hpp"

namespace world
{
    enum class PopupTypes;
}

namespace world::interface::popup
{
    class GenericPopup;
    class ExplorationReward;

    class PopupManager : public core::Singleton <PopupManager>
    {
        container::Pool <ExplorationReward *> exploreChestPopups;

    public:
        void Configure();

        void AddPopup(PopupTypes);

        void RemovePopup(PopupTypes, GenericPopup *);
    };
}