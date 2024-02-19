#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class LayoutGroup;

namespace world::settlement
{
    class Settlement;
}

namespace world::interface
{
    enum class SettlementMenuOptions;

    class SettlementMenu : public Element
    {
        Element *border;

        Text *nameLabel;

        Text *attitudeLabel;

        LayoutGroup *optionLayout;

        settlement::Settlement *currentSettlement {nullptr};

        void HandleConfigure() override;

        void HandleUpdate() override;

    public:
        void Setup(settlement::Settlement *);

<<<<<<< HEAD
        void ProcessOptionInput();
=======
        void ProcessOptionInput(SettlementMenuOptions);
>>>>>>> testBranch

        settlement::Settlement *GetCurrentSettlement() const {return currentSettlement;}
    };
}