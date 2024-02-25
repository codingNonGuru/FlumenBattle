#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Animation.h"
#include "FlumenEngine/Core/Transform.hpp"

#include "ConquestPopup.h"
#include "FlumenBattle/World/Group/Quest.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto BORDER_INNER_OFFSET = Size(4, 4);

static const auto TEXT_COLOR = Color::RED * 0.5f;

static constexpr auto CLOSE_POPUP_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_RETURN;

static const auto ANIMATION_LENGTH = 0.3f;

void ConquestPopup::HandleConfigure()
{
    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - BORDER_INNER_OFFSET, 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true} 
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    infoLabel = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {Position2(0.0f, -40.0f), this}
        },
        {
            {"Medium"}, 
            TEXT_COLOR
        }
    );
    infoLabel->Enable();

    continueLabel = ElementFactory::BuildText
    (
        {
            drawOrder_ + 1, 
            {Position2(0.0f, 60.0f), this}
        },
        {
            {"Large"}, 
            TEXT_COLOR, 
            "[Continue]"
        }
    );
    continueLabel->Enable();

    UpdatePositionConstantly();
}

void ConquestPopup::HandleSetupAnimations()
{
    openAnimation_->SetLength(ANIMATION_LENGTH);

    auto property = openAnimation_->AddProperty({openAnimation_, &GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 0.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, opacity_);

    property = openAnimation_->AddProperty({openAnimation_, &border->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 0.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 1.0f);

    property = openAnimation_->AddProperty({openAnimation_, &infoLabel->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 0.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 1.0f);

    property = openAnimation_->AddProperty({openAnimation_, &continueLabel->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 0.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 1.0f);

    property = openAnimation_->AddProperty({openAnimation_, &basePosition_.y});

    property->AddKey()->Initialize(0.0f, 200.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    closeAnimation_->SetLength(ANIMATION_LENGTH);

    property = closeAnimation_->AddProperty({closeAnimation_, &GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, opacity_);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    property = closeAnimation_->AddProperty({closeAnimation_, &border->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    property = closeAnimation_->AddProperty({closeAnimation_, &infoLabel->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    property = closeAnimation_->AddProperty({closeAnimation_, &continueLabel->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    property = closeAnimation_->AddProperty({closeAnimation_, &basePosition_.y});

    property->AddKey()->Initialize(0.0f, 0.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, -200.0f);
}

void ConquestPopup::HandleClosePressed()
{
    InputHandler::UnregisterEvent(CLOSE_POPUP_INPUT_KEY);

    Close();
}

void ConquestPopup::Setup(settlement::Settlement *settlement)
{
    auto text = Phrase("My liege, you have conquered ") << settlement->GetName() << ".";
    infoLabel->Setup(text);

    InputHandler::RegisterEvent(CLOSE_POPUP_INPUT_KEY, {this, &ConquestPopup::HandleClosePressed});
}