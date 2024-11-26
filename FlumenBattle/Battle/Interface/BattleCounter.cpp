#include "FlumenCore/Delegate/Delegate.hpp"
#include "FlumenCore/Delegate/Event.hpp"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Animation/AnimationProperty.hpp"
#include "FlumenEngine/Animation/AnimationKey.hpp"
#include "FlumenEngine/Animation/Animator.hpp"
#include "FlumenEngine/Animation/Animation.hpp"
#include "FlumenEngine/Animation/AnimationEvent.hpp"

#include "BattleCounter.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/Types.hpp"

using namespace battle::interface;

static const auto ANIMATION_NAME = "Lift";

static const auto ANIMATION_LENGTH = 2.0f;

static const auto COUNTER_END_HEIGHT = -100.0f;

static const auto FONT_SIZE = "Large";

static const auto BASE_COUNTER_SCALE = Scale2(1.0f, 1.0f);

static const auto TEXTURE_NAME = "WhiteDotBackdrop";

void BattleCounter::HandleConfigure()
{
    counter = ElementFactory::BuildElement <world::interface::Counter>
    (
        {drawOrder_ + 3, {this}, {TEXTURE_NAME, false}}
    );
    counter->Setup(&value, BASE_COUNTER_SCALE, FONT_SIZE);
    counter->UpdatePositionConstantly();
    counter->Enable();

    ConfigureAnimation();
}

void BattleCounter::ConfigureAnimation()
{
    animation = animator_->AddAnimation(new Animation(ANIMATION_LENGTH), ANIMATION_NAME);

    auto property = animation->AddProperty({animation, &counter->GetBasePosition().y});

    property->AddKey()->Initialize(0.0f, 0.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, COUNTER_END_HEIGHT);

    property = animation->AddProperty({animation, &counter->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    auto finishEvent = animation->GetFinishEvent();
    finishEvent->GetActions() += {this, &BattleCounter::Disable};
}

void BattleCounter::Setup(int newValue, Combatant *newCombatant) 
{
    value = newValue; 
    
    combatant = newCombatant;

    FollowWorldPosition(&combatant->GetTile()->Position, Cameras::BATTLE, Scale2(0.0f, -2.0f), Scale2(0.0f, -40.0f));
}

void BattleCounter::HandleEnable()
{
    animation->Play();
}

void BattleCounter::HandleDisable()
{
    BattleInterface::Get()->RemoveDamageCounter(this);
}