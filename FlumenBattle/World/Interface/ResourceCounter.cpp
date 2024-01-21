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
    int value;
    switch(valueOrigin)
    {
    case ValueOrigins::POINTER:
        value = *valuePointer;
        break;
    case ValueOrigins::FUNCTION:
        value = valueFetcher();
        break;
    case ValueOrigins::STRING:
        text << valueString;
        break;    
    }

    if(valueOrigin != ValueOrigins::STRING)
    {
        if(isSignSensitive == true && value > 0)
        {
            text << "+";
        }

        text << value;
    }

    label->Setup(text);
}

void ResourceCounter::Setup(Word name, const int *newValue, Word fontSize, Scale2 textureScale)
{
    valuePointer = newValue;

    icon->GetSprite()->SetTexture(name);

    icon->GetSprite()->SetTextureSize(textureScale);

    label->SetFont({DEFAULT_FONT_TYPE, fontSize});

    valueOrigin = ValueOrigins::POINTER;
}

void ResourceCounter::Setup(Word name, std::function <int(void)> newFetcher, Word fontSize, Scale2 textureScale)
{
    valueFetcher = newFetcher;

    icon->GetSprite()->SetTexture(name);

    icon->GetSprite()->SetTextureSize(textureScale);

    label->SetFont({DEFAULT_FONT_TYPE, fontSize});

    valueOrigin = ValueOrigins::FUNCTION;
}

void ResourceCounter::Setup(Word name, Word string, Word fontSize, Scale2 textureScale)
{
    valueString = string;

    icon->GetSprite()->SetTexture(name);

    icon->GetSprite()->SetTextureSize(textureScale);

    label->SetFont({DEFAULT_FONT_TYPE, fontSize});

    valueOrigin = ValueOrigins::STRING;
}