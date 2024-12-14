#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Animation.h"

#include "ActionPopup.h"
#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"

using namespace world::interface;

static const auto ANIMATION_NAME = "Lift";

static const auto ANIMATION_LENGTH = 1.0f;

static const auto COUNTER_END_HEIGHT = -100.0f;

static const auto FONT_SIZE = "Medium";

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto BACKDROP_SIZE_DELTA = 10;

void ActionPopup::HandleConfigure()
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

void ActionPopup::ConfigureAnimation()
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
    finishEvent->GetActions() += {this, &ActionPopup::Disable};
}

void ActionPopup::HandleEnable()
{
    animation->Play();
}

void ActionPopup::HandleDisable()
{
    WorldInterface::Get()->RemoveActionPopup(this);
}

void ActionPopup::HandleUpdate()
{
    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    followedPosition = playerGroup->GetVisualPosition();
}

void ActionPopup::Setup(group::GroupActions actionType, bool hasActionStarted)
{
    switch(actionType)
    {
    case group::GroupActions::TRAVEL:
        label->Setup("Travel");
        break;
    case group::GroupActions::TAKE_LONG_REST:
        label->Setup("Long rest");
        break;
    case group::GroupActions::TAKE_SHORT_REST:
        label->Setup("Quick rest");
        break;
    case group::GroupActions::BYPASS_DEFENCES:
        label->Setup("Sneak");
        break;
    case group::GroupActions::ENGAGE:
        label->Setup("Engage");
        break;
    case group::GroupActions::PERSUADE:
        label->Setup("Persuade");
        break;
    case group::GroupActions::SEARCH:
        label->Setup("Search");
        break;
    case group::GroupActions::FIGHT:
        label->Setup("Fight");
        break;
    case group::GroupActions::DISENGAGE:
        label->Setup("Disengage");
        break;
    case group::GroupActions::FORAGE:
        label->Setup("Forage");
        break;
    case group::GroupActions::LOOT_SETTLEMENT:
        label->Setup("Loot");
        break;
    case group::GroupActions::PILLAGE_SETTLEMENT:
        label->Setup("Pillage");
        break;
    }

    backdrop->SetWidth(label->GetWidth() + BACKDROP_SIZE_DELTA);

    static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

    followedPosition = playerGroup->GetVisualPosition();

    FollowWorldPosition(&followedPosition, Cameras::WORLD, Scale2(0.0f, -2.0f), Scale2(0.0f, -40.0f));
}