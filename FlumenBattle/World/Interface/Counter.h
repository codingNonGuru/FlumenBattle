#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::interface
{
    class Counter : public Element
    {
        static constexpr auto DEFAULT_FONT_SIZE = "Small";

        static constexpr auto DEFAULT_FONT_TYPE = "JSLAncient";

        Text *label;

        const int *value;

        void HandleConfigure() override;    

        void HandleUpdate() override;  

    public:
        void Setup(const int *, Word = DEFAULT_FONT_TYPE, Word = DEFAULT_FONT_SIZE);
    };
}
