#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Animation/AnimationProperty.hpp"
#include "FlumenEngine/Animation/AnimationKey.hpp"
#include "FlumenEngine/Animation/Animator.hpp"
#include "FlumenEngine/Animation/Animation.hpp"
#include "FlumenEngine/Animation/AnimationEvent.hpp"

#include "ItemPopup.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Group/ItemChange.h"

using namespace world::interface;

static const auto ANIMATION_NAME = "Lift";

static const auto ANIMATION_LENGTH = 1.0f;

static const auto COUNTER_END_HEIGHT = -100.0f;

static const auto FONT_SIZE = "Large";

static const auto SUCCESS_COLOR = Color::GREEN * 0.5f;

static const auto FAILURE_COLOR = Color::RED * 0.8f;

static const auto TEXTURE_NAME = "Coin";

static const auto BACKDROP_PADDING = Size(6, 4);

void ItemPopup::HandleConfigure()
{
    backdrop = ElementFactory::BuildElement <LayoutGroup>
    (
        {size_, drawOrder_, {this}, SpriteDescriptor{false}}
    );
    backdrop->SetPadding(BACKDROP_PADDING);
    backdrop->UpdatePositionConstantly();
    backdrop->Enable();

    label = ElementFactory::BuildText
    (
        {drawOrder_ + 1, {backdrop}}, {{FONT_SIZE}, SUCCESS_COLOR}
    );
    label->Enable();

    itemIcon = ElementFactory::BuildElement <Element>
    (
        {Size(32, 32), drawOrder_ + 1, {backdrop}, SpriteDescriptor{TEXTURE_NAME, false}}
    );
    itemIcon->Enable();

    ConfigureAnimation();
}

void ItemPopup::ConfigureAnimation()
{
    animation = animator_->AddAnimation(new Animation(ANIMATION_LENGTH), ANIMATION_NAME);

    auto property = animation->AddProperty({animation, &backdrop->GetBasePosition().y});

    property->AddKey()->Initialize(0.0f, 0.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, COUNTER_END_HEIGHT);

    property = animation->AddProperty({animation, &backdrop->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    property = animation->AddProperty({animation, &label->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    property = animation->AddProperty({animation, &itemIcon->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    auto finishEvent = animation->GetFinishEvent();
    finishEvent->GetActions() += {this, &ItemPopup::Disable};
}

void ItemPopup::HandleEnable()
{
    animation->Play();
}

void ItemPopup::HandleDisable()
{
    WorldInterface::Get()->RemoveItemPopup(this);
}

void ItemPopup::HandleUpdate()
{
    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    followedPosition = playerGroup->GetVisualPosition();
}

void ItemPopup::Setup(const group::ItemChange &itemChange)
{
    label->Setup(ShortWord() << (itemChange.Amount > 0 ? "+" : "") << itemChange.Amount);

    label->SetColor(itemChange.Amount > 0 ? SUCCESS_COLOR : FAILURE_COLOR);

    itemIcon->SetTexture(character::Item::GetTextureName(itemChange.Type));

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    followedPosition = playerGroup->GetVisualPosition();

    FollowWorldPosition(&followedPosition, Cameras::WORLD, Scale2(0.0f, -2.0f), Scale2(0.0f, -40.0f));
}