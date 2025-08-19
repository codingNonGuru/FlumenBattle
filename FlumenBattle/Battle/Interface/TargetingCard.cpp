#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Animation.h"

#include "TargetingCard.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/Battle/Combatant.h"

using namespace battle::interface;

static const auto ANIMATION_NAME = "Emerge";

static const auto ANIMATION_LENGTH = 0.4f;

static const auto FONT_SIZE = "Medium";

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const SpriteDescriptor BORDER_SPRITE = {"panel-border-007", true};

static const auto BORDER_OPACITY = Opacity(0.8f);

void TargetingCard::HandleConfigure()
{
    border = ElementFactory::BuildElement <Element>
    (
        {size_, drawOrder_, {this}, BORDER_SPRITE, BORDER_OPACITY}
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    /*label = ElementFactory::BuildText
    (
        {drawOrder_ + 1, {backdrop}}, {{FONT_SIZE}, TEXT_COLOR}
    );
    label->Enable();*/

    //backdrop->SetHeight(label->GetHeight() + BACKDROP_SIZE_DELTA);

    ConfigureAnimation();
}

void TargetingCard::ConfigureAnimation()
{
    /*animation = animator_->AddAnimation(new Animation(ANIMATION_LENGTH), ANIMATION_NAME);

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
    finishEvent->GetActions() += {this, &ConditionPopup::Disable};*/
}

void TargetingCard::HandleEnable()
{
    //animation->Play();
}

void TargetingCard::HandleDisable()
{
    
}

void TargetingCard::HandleUpdate()
{
    //followedPosition = combatant->GetPosition();
}

void TargetingCard::Setup(const Combatant *combatant, const Combatant *target)
{
    this->combatant = combatant;
    this->target = target;

    //const auto &condition = world::character::ConditionFactory::Get()->Create({conditionType, 0, 0});
    //label->Setup(condition.Type->Name);

    //backdrop->SetWidth(label->GetWidth() + BACKDROP_SIZE_DELTA);

    followedPosition = combatant->GetPosition() * 0.5f + target->GetPosition() * 0.5f;

    //FollowWorldPosition(&followedPosition, Cameras::BATTLE);
    FollowWorldPosition(&followedPosition, Cameras::BATTLE, Scale2(0.0f, 0.0f), Scale2(0.0f, 0.0f));
}