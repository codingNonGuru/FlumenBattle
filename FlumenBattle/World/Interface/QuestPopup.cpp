#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "QuestPopup.h"
#include "FlumenBattle/World/Group/Quest.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto BORDER_INNER_OFFSET = Size(4, 4);

static const auto TEXT_COLOR = Color::RED * 0.5f;

static constexpr auto CLOSE_POPUP_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_RETURN;

void QuestPopup::HandleConfigure()
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
            TEXT_COLOR, 
            ""
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

void QuestPopup::HandleClosePressed()
{
    InputHandler::UnregisterEvent(CLOSE_POPUP_INPUT_KEY);

    Disable();
}

void QuestPopup::Setup(const group::Quest &quest, bool hasStarted)
{
    if(hasStarted == true)
    {
        if(quest.Type == group::QuestTypes::DELIVER_ITEM)
        {
            auto text = Phrase("My liege, thou must deliver this item\nunto ") << quest.Data.TargetSettlement->GetName() << " no sooner than\n" << quest.DayCount << " days from now on.";
            infoLabel->Setup(text);
        }
        else
        {
            infoLabel->Setup("Aloha");
        }
    }
    else
    {
        if(quest.Type == group::QuestTypes::DELIVER_ITEM)
        {
            auto text = Phrase("Thy feet were swift, my liege.\nThe item was promptly conveyed.");
            infoLabel->Setup(text);
        }
        else
        {
            infoLabel->Setup("Aloha");
        }
    }

    InputHandler::RegisterEvent(CLOSE_POPUP_INPUT_KEY, {this, &QuestPopup::HandleClosePressed});
}