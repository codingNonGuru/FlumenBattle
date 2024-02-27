#include "SDL2/SDL.h"

#include "FlumenEngine/Core/InputHandler.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "RecruitmentItem.h"
#include "FlumenBattle/World/Interface/RecruitmentMenu.h"
#include "FlumenBattle/World/Character/RecruitHandler.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/World/Character/CharacterFactory.h"

using namespace world::interface;

static auto BORDER_COLOR = Color::RED * 0.25f;

static auto TEXT_COLOR = Color::RED * 0.5f;

static constexpr auto BASE_OPTION_OPACITY = 0.5f;

static constexpr auto HOVERED_OPTION_OPACITY = 1.0f;

static constexpr const char *ROMAN_LITERALS[12] = {"I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX", "X", "XI", "XII"};

void RecruitmentItem::HandleConfigure() 
{
    SetSpriteColor(BORDER_COLOR);

    SetInteractivity(true);

    SetOpacity(BASE_OPTION_OPACITY);

    classLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, -60.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}},
        {{"Medium"}, TEXT_COLOR}
    );
    classLabel->Enable();

    raceLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, classLabel}},
        {{"Small"}, TEXT_COLOR}
    );
    raceLabel->Enable();

    levelLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(10.0f, 10.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}},
        {{"Medium"}, TEXT_COLOR}
    );
    levelLabel->Enable();

    recruitLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, this}},
        {{"Medium"}, TEXT_COLOR, "Recruit"}
    );

    costCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_ + 1, {Position2(0.0f, -20.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}}
    );
    costCounter->Enable();

    icon = ElementFactory::BuildElement <Element>(
        {Size(32, 32), drawOrder_ + 1, {Position2(0.0f, -30.0f), this}, SpriteDescriptor{false}}
    );
    icon->SetTextureScale(Scale2(2.0f));
    icon->Enable();
}

void RecruitmentItem::HandleUpdate()
{
    if(isHovered_ == false || group::HumanMind::Get()->CanRecruitCharacter(*recruitData) == false)
    {
        SetOpacity(BASE_OPTION_OPACITY);

        recruitLabel->Disable();
    }
    else
    {
        SetOpacity(HOVERED_OPTION_OPACITY);

        recruitLabel->Enable();
    }
}

void RecruitmentItem::HandleLeftClick() 
{
    if(group::HumanMind::Get()->CanRecruitCharacter(*recruitData) == false)
        return;

    Disable();

    parent->ProcessInput(recruitData);
}

void RecruitmentItem::Setup(RecruitmentMenu *parent, const character::RecruitData *data)
{
    this->parent = parent;

    this->recruitData = data;

    auto className = utility::GetClassName(recruitData->Class);
    classLabel->Setup(className);

    raceLabel->Setup(data->Race->Name);

    levelLabel->Setup(ROMAN_LITERALS[recruitData->Level - 1]);

    icon->SetTexture(character::CharacterFactory::GetTextureName(data->IconTextureId));

    costCounter->Setup(
        "Coin", 
        &recruitData->Cost,
        "Small"
    );
}