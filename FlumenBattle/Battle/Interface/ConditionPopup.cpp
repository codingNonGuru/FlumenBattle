#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Animation.h"

#include "ConditionPopup.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/Battle/Combatant.h"

using namespace battle::interface;

static const auto ANIMATION_NAME = "Lift";

static const auto ANIMATION_LENGTH = 3.0f;

static const auto COUNTER_END_HEIGHT = -100.0f;

static const auto FONT_SIZE = "Medium";

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto BACKDROP_SIZE_DELTA = 10;

void ConditionPopup::HandleConfigure()
{
    backdrop = ElementFactory::BuildElement <Element>
    (
        {size_, drawOrder_, {this}, SpriteDescriptor{false}}
    );
    backdrop->UpdatePositionConstantly();
    backdrop->Enable();

    label = ElementFactory::BuildText
    (
        {drawOrder_ + 1, {backdrop}}, {{FONT_SIZE}, TEXT_COLOR}
    );
    label->Enable();

    backdrop->SetHeight(label->GetHeight() + BACKDROP_SIZE_DELTA);

    ConfigureAnimation();
}

void ConditionPopup::ConfigureAnimation()
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
    finishEvent->GetActions() += {this, &ConditionPopup::Disable};
}

void ConditionPopup::HandleEnable()
{
    animation->Play();
}

void ConditionPopup::HandleDisable()
{
    BattleInterface::Get()->RemoveConditionPopup(this);
}

void ConditionPopup::HandleUpdate()
{
    followedPosition = combatant->GetPosition();
}

void ConditionPopup::Setup(const Combatant *combatant, world::character::Conditions conditionType)
{
    this->combatant = combatant;

    const auto &condition = world::character::ConditionFactory::Get()->Create({conditionType, 0, 0});
    label->Setup(condition.Type->Name);

    backdrop->SetWidth(label->GetWidth() + BACKDROP_SIZE_DELTA);

    followedPosition = combatant->GetPosition();

    FollowWorldPosition(&followedPosition, Cameras::BATTLE, Scale2(0.0f, -2.0f), Scale2(0.0f, -40.0f));
}