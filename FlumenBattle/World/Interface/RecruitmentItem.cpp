#include "SDL2/SDL.h"

#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "RecruitmentItem.h"
#include "FlumenBattle/World/Interface/RecruitmentMenu.h"

using namespace world::interface;

static auto BORDER_COLOR = Color::RED * 0.25f;

static auto TEXT_COLOR = Color::RED * 0.5f;

static constexpr auto BASE_OPTION_OPACITY = 0.5f;

static constexpr auto HOVERED_OPTION_OPACITY = 1.0f;

void RecruitmentItem::HandleConfigure() 
{
    SetSpriteColor(BORDER_COLOR);

    SetInteractivity(true);

    SetOpacity(BASE_OPTION_OPACITY);

    recruitLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, this}},
        {{"Medium"}, TEXT_COLOR, "Recruit"}
    );
}

void RecruitmentItem::HandleUpdate()
{
    if(isHovered_)
    {
        SetOpacity(HOVERED_OPTION_OPACITY);

        recruitLabel->Enable();
    }
    else
    {
        SetOpacity(BASE_OPTION_OPACITY);

        recruitLabel->Disable();
    }
}

void RecruitmentItem::HandleLeftClick() 
{
    parent->ProcessInput();
}

void RecruitmentItem::Setup(RecruitmentMenu *parent)
{
    this->parent = parent;
}