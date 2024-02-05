#include "FlumenCore/Observer.h"

#include "FlumenEngine/Sound/SoundManager.h"

#include "WorldMixer.h"
#include "FlumenBattle/World/WorldScene.h"

using namespace world::sound;

static constexpr auto START_BATTLE_SOUND = "StartBattle";

WorldMixer::WorldMixer()
{
    *WorldScene::Get()->OnPlayerBattleStarted += [] {
        engine::SoundManager::Get()->PlaySound(START_BATTLE_SOUND);
    };
}