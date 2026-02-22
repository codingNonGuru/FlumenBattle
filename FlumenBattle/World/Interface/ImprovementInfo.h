#pragma once

#include "FlumenEngine/Interface/Element.hpp"

#include "FlumenBattle/World/Settlement/Types.h"

class ProgressBar;

namespace world
{
    namespace tile {struct WorldTile;}

    /*namespace settlement
    {
        struct ImprovementTarget;
    }*/
}

namespace world::interface
{
    class ImprovementInfo : public Element
    {
        settlement::ImprovementTarget target;

        ProgressBar *progressBar;

        bool isCurrentlyBeingBuilt;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(settlement::ImprovementTarget);
    };

    class ImprovementInfoSet : public Element
    {
        container::Array <ImprovementInfo *> infos;

        void HandleConfigure() override;

        void HandleImproveModeEntered();
    };
}