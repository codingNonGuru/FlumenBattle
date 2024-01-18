#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"

#include "ResourceCounter.h"

using namespace world::interface;

void ResourceCounter::HandleConfigure()
{
    icon = ElementFactory::BuildElement <Element>(
        {Size(32, 32), drawOrder_ + 1, {Position2(), this}, {"Sprite"}}
    );
    icon->Enable();

    label = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_LEFT, icon}},
        {{DEFAULT_FONT_TYPE, DEFAULT_FONT_SIZE}, Color::RED * 0.5f, "20"}
    );
    label->SetAlignment(Text::Alignments::LEFT);
    label->Enable();
}

void ResourceCounter::HandleUpdate()
{
    Phrase text(*value);
    label->Setup(text);
}

void ResourceCounter::Setup(Word name, const int *newValue, Word fontType, Word fontSize)
{
    value = newValue;

    icon->GetSprite()->SetTexture(name);

    label->SetFont({fontType, fontSize});
}