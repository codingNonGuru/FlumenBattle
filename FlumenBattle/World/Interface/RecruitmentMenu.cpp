#include "SDL2/SDL.h"

#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "RecruitmentMenu.h"
#include "FlumenBattle/World/Interface/RecruitmentItem.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::interface;

#define MAXIMUM_ITEM_COUNT 8

static auto BORDER_COLOR = Color::RED * 0.25f;

static auto TEXT_COLOR = Color::RED * 0.5f;

static constexpr auto CLOSE_POPUP_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_RETURN;

void RecruitmentMenu::HandleConfigure() 
{
    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - Size(4, 4), 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true}
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    titleLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 15.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}},
        {{"Large"}, TEXT_COLOR, "Recruit heroes"}
    );
    titleLabel->Enable();

    itemLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {this}
        }
    );
    itemLayout->SetDistancing(5, 10.0f);
    itemLayout->Enable();

    items.Initialize(MAXIMUM_ITEM_COUNT);

    for(auto i = 0; i < MAXIMUM_ITEM_COUNT; ++i)
    {
        auto item = items.Add();

        *item = ElementFactory::BuildElement <RecruitmentItem>(
            {Size(120, 200), drawOrder_ + 1, {itemLayout}, {"panel-border-007", true}}
        );
    }
}

void RecruitmentMenu::HandleEnable()
{
    auto item = items.GetStart();
    for(int i = 0; i < 3; ++i)
    {
        (*item)->Enable();
        (*item)->Setup(this);

        item++;
    }

    InputHandler::RegisterEvent(CLOSE_POPUP_INPUT_KEY, {this, &RecruitmentMenu::HandleClosePressed});
}

void RecruitmentMenu::HandleClosePressed()
{
    InputHandler::UnregisterEvent(CLOSE_POPUP_INPUT_KEY);

    for(auto &item : items)
    {
        item->Disable();
    }

    Disable();
}

void RecruitmentMenu::ProcessInput()
{
    group::HumanMind::Get()->RecruitCharacter();
}