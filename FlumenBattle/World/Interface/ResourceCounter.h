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

        bool isSignSensitive {false};

        void HandleConfigure() override;    

        void HandleUpdate() override;  

    public:
        void Setup(Word, const int *, Word = DEFAULT_FONT_SIZE, Scale2 = Scale2(1.0f));

        void Setup(Word, std::function <int(void)>, Word = DEFAULT_FONT_SIZE, Scale2 = Scale2(1.0f));

        void MakeSignSensitive() {isSignSensitive = true;}
    };
}