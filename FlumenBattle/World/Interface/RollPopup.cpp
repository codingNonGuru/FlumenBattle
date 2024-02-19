#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Animation/AnimationProperty.hpp"
#include "FlumenEngine/Animation/AnimationKey.hpp"
#include "FlumenEngine/Animation/Animator.hpp"
#include "FlumenEngine/Animation/Animation.hpp"
#include "FlumenEngine/Animation/AnimationEvent.hpp"

#include "RollPopup.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"

using namespace world::interface;

static const auto ANIMATION_NAME = "Lift";

static const auto ANIMATION_LENGTH = 1.0f;

static const auto COUNTER_END_HEIGHT = -200.0f;

static const auto FONT_SIZE = "Medium";

static const auto SUCCESS_COLOR = Color::GREEN * 0.7f;

static const auto FAILURE_COLOR = Color::RED;

static const auto BACKDROP_PADDING = Size(8, 4);

static const auto TEXTURE_NAME = "Dice";

void RollPopup::HandleConfigure()
{
    backdrop = ElementFactory::BuildElement <LayoutGroup>
    (
        {size_, drawOrder_, {this}, SpriteDescriptor{false}}
    );
    backdrop->UpdatePositionConstantly();
    backdrop->SetPadding(BACKDROP_PADDING);
    backdrop->Enable();

    diceIcon = ElementFactory::BuildElement <Element>
    (
        {Size(32, 32), drawOrder_ + 1, {backdrop}, SpriteDescriptor{TEXTURE_NAME, false}}
    );
    diceIcon->Enable();

    label = ElementFactory::BuildText
    (
        {drawOrder_ + 1, {backdrop}}, {{FONT_SIZE}, SUCCESS_COLOR}
    );
    label->Enable();

    ConfigureAnimation();
}

void RollPopup::ConfigureAnimation()
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

    auto finishEvent = animation->GetFinishEvent();
    finishEvent->GetActions() += {this, &RollPopup::Disable};
}

void RollPopup::HandleEnable()
{
    animation->Play();
}

void RollPopup::HandleDisable()
{
    WorldInterface::Get()->RemoveRollPopup(this);
}

void RollPopup::HandleUpdate()
{
    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    followedPosition = playerGroup->GetVisualPosition();
}

void RollPopup::Setup(utility::Success roll)
{
    label->Setup(ShortWord() << (roll.Roll + roll.Modifier));

    label->SetColor(roll.IsAnySuccess() == true ? SUCCESS_COLOR : FAILURE_COLOR);

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    followedPosition = playerGroup->GetVisualPosition();

    FollowWorldPosition(&followedPosition, Cameras::WORLD, Scale2(0.0f, -2.0f), Scale2(0.0f, -40.0f));
}