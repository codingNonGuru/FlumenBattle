#include "FlumenCore/Observer.h"

#include "FlumenEngine/Sound/SoundManager.h"

#include "WorldMixer.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/HumanMind.h"

using namespace world::sound;

static constexpr auto START_BATTLE_SOUND = "StartBattle";

static const auto COIN_SOUNDS = container::Array {"Coin", "Coin2", "Coin3", "Coin4"};

WorldMixer::WorldMixer()
{
    *WorldScene::Get()->OnPlayerBattleStarted += [] {
        engine::SoundManager::Get()->PlaySound(START_BATTLE_SOUND);
    };

    *WorldScene::Get()->OnPlayerEncounterFinished += [] {
        auto sound = COIN_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*sound);
    };

<<<<<<< HEAD
    *group::HumanMind::Get()->OnItemAdded += [] {
=======
    group::HumanMind::Get()->OnItemAdded += [] {
>>>>>>> testBranch
        auto sound = COIN_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*sound);
    };

<<<<<<< HEAD
    *group::HumanMind::Get()->OnItemSold += [] {
=======
    group::HumanMind::Get()->OnItemSold += [] {
>>>>>>> testBranch
        auto sound = COIN_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*sound);
    };
}