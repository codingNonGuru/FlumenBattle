#include "FlumenEngine/Interface/Text.hpp"

#include "ExplorationReward.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/WorldTile.h"
#include "FlumenBattle/World/Group/Group.h"

using namespace world::interface::popup;

void ExplorationReward::Setup()
{
    popupType = PopupTypes::EXPLORATION_REWARD;

    label->Disable();

    backdrop->Enable();
    backdrop->SetRendering(false);

    icon->Enable();
    icon->SetBasePosition(Position2(0.0f));
    icon->SetTexture("Chest");
    icon->SetTextureScale(1.7f);

    auto tile = polity::HumanMind::Get()->GetLastExploredTile();

    followedPosition = tile->Position;

    FollowWorldPosition(&followedPosition, Cameras::WORLD, Scale2(0.0f, -2.0f), Scale2(0.0f, -40.0f));
}