#include "SDL2/SDL.h"

#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Animation.h"

#include "RecruitmentMenu.h"
#include "FlumenBattle/World/Interface/RecruitmentItem.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Character/RecruitHandler.h"

using namespace world::interface;

#define MAXIMUM_ITEM_COUNT 8

static auto BORDER_COLOR = Color::RED * 0.25f;

static auto TEXT_COLOR = Color::RED * 0.5f;

static constexpr auto CLOSE_POPUP_INPUT_KEY = SDL_Scancode::SDL_SCANCODE_RETURN;

static const auto ANIMATION_LENGTH = 0.3f;

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
            {Position2(10.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}
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

    group::HumanMind::Get()->OnCurrentRecruitPoolUpdate += {this, &RecruitmentMenu::HandlePoolUpdate};
}

void RecruitmentMenu::HandleEnable()
{
    RefreshItemList();

    InputHandler::RegisterEvent(CLOSE_POPUP_INPUT_KEY, {this, &RecruitmentMenu::HandleClosePressed});
}

void RecruitmentMenu::HandleSetupAnimations()
{
    UpdatePositionConstantly();

    openAnimation_->SetLength(ANIMATION_LENGTH);

    auto property = openAnimation_->AddProperty({openAnimation_, &GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 0.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, opacity_);

    property = openAnimation_->AddProperty({openAnimation_, &border->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 0.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 1.0f);

    property = openAnimation_->AddProperty({openAnimation_, &titleLabel->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 0.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 1.0f);

    property = openAnimation_->AddProperty({openAnimation_, &basePosition_.y});

    property->AddKey()->Initialize(0.0f, 150.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    closeAnimation_->SetLength(ANIMATION_LENGTH);

    property = closeAnimation_->AddProperty({closeAnimation_, &GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, opacity_);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    property = closeAnimation_->AddProperty({closeAnimation_, &border->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    property = closeAnimation_->AddProperty({closeAnimation_, &titleLabel->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    property = closeAnimation_->AddProperty({closeAnimation_, &basePosition_.y});

    property->AddKey()->Initialize(0.0f, 0.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, -150.0f);
}

void RecruitmentMenu::HandleClosePressed()
{
    InputHandler::UnregisterEvent(CLOSE_POPUP_INPUT_KEY);

    for(auto &item : items)
    {
        item->Disable();
    }

    Close();
}

void RecruitmentMenu::HandlePoolUpdate()
{
    RefreshItemList();
}

void RecruitmentMenu::RefreshItemList()
{
    for(auto &item : items)
    {
        item->Disable();
    }

    const auto &recruits = character::RecruitHandler::Get()->GetRecruitPool(settlement);

    auto item = items.GetStart();
    for(auto &recruit : recruits)
    {
        (*item)->Enable();
        (*item)->Setup(this, &recruit);

        item++;
    }
}

void RecruitmentMenu::Setup(settlement::Settlement *settlement)
{
    this->settlement = settlement;
}

void RecruitmentMenu::ProcessInput(const character::RecruitData *recruitData)
{
    group::HumanMind::Get()->RecruitCharacter(*recruitData);
}