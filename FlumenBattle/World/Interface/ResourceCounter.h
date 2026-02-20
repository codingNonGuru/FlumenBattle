#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;

namespace world::interface
{
    class ResourceCounter : public Element
    {
        static constexpr auto DEFAULT_FONT_SIZE = "Small";

        Element *icon;

        Text *label;

        enum class ValueOrigins
        {
            STRING, 
            POINTER,
            FUNCTION,
            STRING_FUNCTION
        } valueOrigin;

        Word valueString;

        const int *valuePointer {nullptr};

        std::function <int(void)> valueFetcher;

        std::function <Word(void)> stringFetcher;

        bool isSignSensitive {false};

        Position2 offset {0.0f, 0.0f};

        void HandleConfigure() override;    

        void HandleUpdate() override;  

    public:
        void Setup(Word, const int *, Word = DEFAULT_FONT_SIZE, Scale2 = Scale2(1.0f));

        void Setup(Word, std::function <int(void)>, Word = DEFAULT_FONT_SIZE, Scale2 = Scale2(1.0f));

        void Setup(Word, Word, Word = DEFAULT_FONT_SIZE, Scale2 = Scale2(1.0f));

        void Setup(Word, std::function <Word(void)>, Word = DEFAULT_FONT_SIZE, Scale2 = Scale2(1.0f));

        void SetOffset(float);

        void SetOffset(Position2);

        void SetIconScale(float);

        void MakeSignSensitive() {isSignSensitive = true;}

        void SetIconTexture(Word);
    };
}