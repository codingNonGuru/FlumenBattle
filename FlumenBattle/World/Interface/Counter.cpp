#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"

#include "Counter.h"

using namespace world::interface;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void Counter::HandleConfigure()
{
    label = ElementFactory::BuildText(
        {drawOrder_ + 1, {this}},
        {{DEFAULT_FONT_SIZE}, TEXT_COLOR}
    );
    label->Enable();

    GetSprite()->SetTextureSize(backdropScale);
}

void Counter::HandleUpdate()
{
    auto finalValue = [&]
    {   
        if(valueSource == nullptr)
            return value;
        else
            return *valueSource;
    } ();

    auto text = ShortWord();
    if(isSigned == true && finalValue > 0)
    {
        text = text << "+";
    }

    label->Setup(text << finalValue, backdropScale.x * textScale);

    //auto finalValue = valueSource != nullptr ? *valueSource : value;

    if(finalValue > 9)
    {
        GetSprite()->SetTextureSize(backdropScale * BACKDROP_ENLARGE_FACTOR);
    }
    else
    {
        GetSprite()->SetTextureSize(backdropScale);
    }
}

void Counter::Setup(const int *newValueSource, Scale2 newScale, Word fontSize)
{
    valueSource = newValueSource;

    label->SetFont({fontSize});

    backdropScale = newScale;
}

void Counter::Setup(int newValue, Scale2 newScale, Word fontSize)
{
    value = newValue;

    valueSource = nullptr;

    label->SetFont({fontSize});

    backdropScale = newScale;
}

void Counter::Setup(Scale2 newScale, Word fontSize)
{
    label->SetFont({fontSize});

    backdropScale = newScale;
}

void Counter::SetTextColor(Color color)
{
    label->SetColor(color);
}