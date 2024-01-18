#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"

#include "Counter.h"

using namespace world::interface;

static const auto BACKDROP_SCALE = Scale2(0.8f, 0.8f);

static const auto BACKDROP_SCALE_ENLARGED = Scale2(0.9f, 0.9f);

static const auto TEXT_COLOR = Color::RED * 0.5f;

void Counter::HandleConfigure()
{
    label = ElementFactory::BuildText(
        {Size(), drawOrder_ + 1, {Position2(), this}},
        {{DEFAULT_FONT_TYPE, DEFAULT_FONT_SIZE}, TEXT_COLOR, "5"}
    );
    label->Enable();

    GetSprite()->SetTextureSize(BACKDROP_SCALE);
}

void Counter::HandleUpdate()
{
    Phrase text(*value);
    label->Setup(text);

    if(*value > 9)
    {
        GetSprite()->SetTextureSize(BACKDROP_SCALE_ENLARGED);
    }
    else
    {
        GetSprite()->SetTextureSize(BACKDROP_SCALE);
    }
}

void Counter::Setup(const int *newValue, Word fontType, Word fontSize)
{
    value = newValue;

    label->SetFont({fontType, fontSize});
}