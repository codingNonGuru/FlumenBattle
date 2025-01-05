#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"

#include "ResourceCounter.h"

using namespace world::interface;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void ResourceCounter::HandleConfigure()
{
    icon = ElementFactory::BuildElement <Element>
    (
        {drawOrder_ + 1, {this}, SpriteDescriptor{false}}
    );
    icon->Enable();

    label = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_LEFT, icon}},
        {{DEFAULT_FONT_SIZE}, TEXT_COLOR}
    );
    label->SetAlignment(Text::Alignments::LEFT);
    label->Enable();
}

void ResourceCounter::HandleUpdate()
{
    if(valueOrigin == ValueOrigins::STRING_FUNCTION)
    {
        Phrase text(stringFetcher());
        label->Setup(text);
        return;
    }

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

    icon->AdjustSizeToTexture();

    label->SetFont({fontSize});

    valueOrigin = ValueOrigins::POINTER;
}

void ResourceCounter::Setup(Word name, std::function <int(void)> newFetcher, Word fontSize, Scale2 textureScale)
{
    valueFetcher = newFetcher;

    icon->GetSprite()->SetTexture(name);

    icon->GetSprite()->SetTextureSize(textureScale);

    icon->AdjustSizeToTexture();

    label->SetFont({fontSize});

    valueOrigin = ValueOrigins::FUNCTION;
}

void ResourceCounter::Setup(Word name, Word string, Word fontSize, Scale2 textureScale)
{
    valueString = string;

    icon->GetSprite()->SetTexture(name);

    icon->GetSprite()->SetTextureSize(textureScale);

    icon->AdjustSizeToTexture();

    label->SetFont({fontSize});

    valueOrigin = ValueOrigins::STRING;
}

void ResourceCounter::Setup(Word name, std::function <Word(void)> newFetcher, Word fontSize, Scale2 textureScale)
{
    stringFetcher = newFetcher;

    icon->GetSprite()->SetTexture(name);

    icon->GetSprite()->SetTextureSize(textureScale);

    icon->AdjustSizeToTexture();

    label->SetFont({fontSize});

    valueOrigin = ValueOrigins::STRING_FUNCTION;
}

void ResourceCounter::SetOffset(float newOffset) 
{
    offset = newOffset;

    label->SetBasePosition({offset, 0.0f});
}

void ResourceCounter::SetIconScale(float scale)
{
    icon->GetSprite()->SetTextureSize(Scale2(scale));

    icon->AdjustSizeToTexture();
}

void ResourceCounter::SetIconTexture(Word textureName)
{
    icon->SetTexture(textureName);

    icon->AdjustSizeToTexture();
}