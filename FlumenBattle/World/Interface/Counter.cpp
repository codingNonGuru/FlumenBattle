#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"

#include "Counter.h"

using namespace world::interface;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void Counter::HandleConfigure()
{
    label = ElementFactory::BuildText(
        {Size(), drawOrder_ + 1, {Position2(), this}},
        {{DEFAULT_FONT_TYPE, DEFAULT_FONT_SIZE}, TEXT_COLOR, "5"}
    );
    label->Enable();

    GetSprite()->SetTextureSize(backdropScale);
}

void Counter::HandleUpdate()
{
    Phrase text(*value);
    label->Setup(text);

    if(*value > 9)
    {
        GetSprite()->SetTextureSize(backdropScale * BACKDROP_ENLARGE_FACTOR);
    }
    else
    {
        GetSprite()->SetTextureSize(backdropScale);
    }
}

void Counter::Setup(const int *newValue, Scale2 newScale, Word fontSize)
{
    value = newValue;

    label->SetFont({DEFAULT_FONT_TYPE, fontSize});

    backdropScale = newScale;
}