#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::interface
{
    class ResourceCounter : public Element
    {
        static constexpr auto DEFAULT_FONT_SIZE = "Small";

        static constexpr auto DEFAULT_FONT_TYPE = "JSLAncient";

        Element *icon;

        Text *label;

        const int *valuePointer {nullptr};

        std::function <int(void)> valueFetcher;

        void HandleConfigure() override;    

        void HandleUpdate() override;  

    public:
        void Setup(Word, const int *, Word = DEFAULT_FONT_TYPE, Word = DEFAULT_FONT_SIZE);

        void Setup(Word, std::function <int(void)>, Word = DEFAULT_FONT_TYPE, Word = DEFAULT_FONT_SIZE);
    };
}