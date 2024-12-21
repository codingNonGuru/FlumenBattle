#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class ProgressBar;

namespace world
{
    class WorldTile;

    namespace settlement
    {
        class Exploration;
    }
}

namespace world::interface
{
    class ExploreInfo : public Element
    {
        settlement::Exploration *exploration;

        ProgressBar *progressBar;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(settlement::Exploration *);
    };

    class ExploreInfoSet : public Element
    {
        container::Array <ExploreInfo *> infos;

        void HandleConfigure() override;

        void HandleExploreModeEntered();
    };
}