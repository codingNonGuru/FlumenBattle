#include "FlumenCore/Observer.h"

#include "FlumenEngine/Sound/SoundManager.h"

#include "BattleMixer.h"
#include "FlumenBattle/Battle/HumanController.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/World/Character/Character.h"

using namespace battle::sound;

static const auto BOW_DRAW_SOUNDS = container::Array {"BowDraw", "BowDraw2", "BowDraw3"};

static const auto BOW_SHOOT_SOUNDS = container::Array {"BowShoot", "BowShoot2", "BowShoot3"};

static const auto SWING_SOUNDS = container::Array {"Swing", "Swing2", "Swing3"};

void BattleMixer::Initialize()
{
    *HumanController::Get()->OnTargetInitiated += {this, &BattleMixer::HandleTargetingInitiated};

    BattleController::Get()->OnCharacterActed += {this, &BattleMixer::HandleCombatantActed};
}

void BattleMixer::HandleTargetingInitiated()
{
    auto character = BattleController::Get()->GetSelectedCharacter();
    if(character->GetActionRange() > 1)
    {
        auto drawSound = BOW_DRAW_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*drawSound);
    }
}

void BattleMixer::HandleCombatantActed()
{
    auto character = BattleController::Get()->GetSelectedCharacter();
    if(character->GetActionRange() > 1)
    {
        auto bowSound = BOW_SHOOT_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*bowSound);
    }
    else
    {
        auto swingSound = SWING_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*swingSound);
    }
}