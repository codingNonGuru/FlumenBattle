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
        {Size(), drawOrder_ + 1, {Position2(), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_LEFT, icon}},
        {{DEFAULT_FONT_TYPE, DEFAULT_FONT_SIZE}, Color::RED * 0.5f, "20"}
    );
    label->SetAlignment(Text::Alignments::LEFT);
    label->Enable();
}

void ResourceCounter::HandleUpdate()
{
    Phrase text;
    if(valuePointer)
    {
        text << *valuePointer;
    }
    else
    {
        text << valueFetcher();
    }

    label->Setup(text);
}

void ResourceCounter::Setup(Word name, const int *newValue, Word fontSize, Scale2 textureScale)
{
    valuePointer = newValue;

    icon->GetSprite()->SetTexture(name);

    icon->GetSprite()->SetTextureSize(textureScale);

    label->SetFont({DEFAULT_FONT_TYPE, fontSize});
}

void ResourceCounter::Setup(Word name, std::function <int(void)> newFetcher, Word fontSize, Scale2 textureScale)
{
    valueFetcher = newFetcher;

    icon->GetSprite()->SetTexture(name);

    icon->GetSprite()->SetTextureSize(textureScale);

    label->SetFont({DEFAULT_FONT_TYPE, fontSize});
}