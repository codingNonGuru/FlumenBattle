#include "FlumenEngine/Interface/Text.hpp"

#include "BattleStartPopup.h"
#include "FlumenBattle/World/Tile/WorldTile.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Group/Encounter.h"

using namespace world::interface::popup;

void BattleStartPopup::Setup()
{
    popupType = PopupTypes::BATTLE_START;

    label->Disable();

    backdrop->Enable();
    backdrop->SetRendering(false);

    icon->Enable();
    icon->SetBasePosition(Position2(0.0f));
    icon->SetTextureScale(1.7f);
    
    icon->SetTexture("CrossedSwords");

    auto battle = WorldScene::Get()->GetMostRecentEncounter();

    auto position = battle->GetAttacker()->GetVisualPosition() + battle->GetAttacker()->GetVisualPosition();
    position *= 0.5f;

    followedPosition = position;

    FollowWorldPosition(&followedPosition, Cameras::WORLD, Scale2(0.0f, -2.0f), Scale2(0.0f, -40.0f));
}