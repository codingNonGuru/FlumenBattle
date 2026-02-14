#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class ProgressBar;
class Text;

namespace world::settlement
{
    struct Job;
}

namespace world::polity
{
    class WorkInstruction;
}

namespace world::interface::rule
{
    class JobItem;
    class RaceItem;

    class JobHoverInfo : public Element
    {
        Element *border;

        ProgressBar *progressBar;

        Text *progressLabel;

        polity::WorkInstruction *hoveredJob;

        JobItem *hoveredItem;

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(JobItem *, polity::WorkInstruction *);
    };
}