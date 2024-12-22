#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Animation/AnimationProperty.hpp"
#include "FlumenEngine/Animation/AnimationKey.hpp"
#include "FlumenEngine/Animation/Animator.hpp"
#include "FlumenEngine/Animation/Animation.hpp"
#include "FlumenEngine/Animation/AnimationEvent.hpp"

#include "GenericPopup.h"
#include "FlumenBattle/World/Interface/Popup/PopupManager.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world::interface::popup;

static const auto ANIMATION_NAME = "Lift";

static const auto ANIMATION_LENGTH = 1.0f;

static const auto COUNTER_END_HEIGHT = -100.0f;

static const auto FONT_SIZE = "Medium";

static const auto TEXT_COLOR = Color::RED;

void GenericPopup::HandleConfigure()
{
    backdrop = ElementFactory::BuildElement <Element>
    (
        {size_, drawOrder_, {this}, SpriteDescriptor{false}},
        2
    );
    backdrop->UpdatePositionConstantly();
    backdrop->Enable();

    icon = ElementFactory::BuildElement <Element>
    (
        {drawOrder_ + 1, {backdrop}, SpriteDescriptor{false}, Opacity(1.0f)}
    );

    label = ElementFactory::BuildText
    (
        {drawOrder_ + 1, {backdrop}}, {{FONT_SIZE}, TEXT_COLOR}
    );

    ConfigureAnimation();
}

void GenericPopup::ConfigureAnimation()
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

    property = animation->AddProperty({animation, &icon->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    auto finishEvent = animation->GetFinishEvent();
    finishEvent->GetActions() += {this, &GenericPopup::Disable};
}

void GenericPopup::HandleEnable()
{
    animation->Play();
}

void GenericPopup::HandleDisable()
{
    PopupManager::Get()->RemovePopup(popupType, this);
}

void GenericPopup::HandleUpdate()
{
}