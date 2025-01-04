#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class ProgressBar;

namespace world
{
    namespace tile {struct WorldTile;}

    namespace settlement
    {
        struct ImprovementTarget;
    }
}

namespace world::interface
{
    class ImprovementInfo : public Element
    {
        const settlement::ImprovementTarget *target;

        ProgressBar *progressBar;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(const settlement::ImprovementTarget *);
    };

    class ImprovementInfoSet : public Element
    {
        container::Array <ImprovementInfo *> infos;

        void HandleConfigure() override;

        void HandleImproveModeEntered();
    };
}