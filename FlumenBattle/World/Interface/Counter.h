#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::interface
{
    class Counter : public Element
    {
        static constexpr auto DEFAULT_FONT_SIZE = "Small";

        static constexpr auto BACKDROP_SCALE = Scale2(0.8f, 0.8f);

        static constexpr auto BACKDROP_ENLARGE_FACTOR = 1.3f;

        Text *label;

        const int *value {nullptr};

        Scale2 backdropScale;

        void HandleConfigure() override;    

        void HandleUpdate() override;  

    public:
        void Setup(const int *, Scale2 = BACKDROP_SCALE, Word = DEFAULT_FONT_SIZE);
    };
}
