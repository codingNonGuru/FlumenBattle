#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "ConquestPopup.h"
#include "FlumenBattle/World/Group/Quest.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto BORDER_INNER_OFFSET = Size(4, 4);

static const auto TEXT_COLOR = Color::RED * 0.5f;

static constexpr auto CLOSE_POPUP_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_RETURN;

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
}

void ConquestPopup::HandleClosePressed()
{
    InputHandler::UnregisterEvent(CLOSE_POPUP_INPUT_KEY);

    Disable();
}

void ConquestPopup::Setup(settlement::Settlement *settlement)
{
    auto text = Phrase("My liege, you have conquered ") << settlement->GetName() << ".";
    infoLabel->Setup(text);

    InputHandler::RegisterEvent(CLOSE_POPUP_INPUT_KEY, {this, &ConquestPopup::HandleClosePressed});
}