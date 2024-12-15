#include "FlumenCore/Observer.h"

#include "FlumenEngine/Sound/SoundManager.h"

#include "WorldMixer.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Polity/HumanMind.h"

using namespace world::sound;

static constexpr auto START_BATTLE_SOUND = "StartBattle";

static const auto COIN_SOUNDS = container::Array {"Coin", "Coin2", "Coin3", "Coin4"};

static const auto BUILDING_SOUNDS = container::Array {"Hammering1", "Hammering2"};

WorldMixer::WorldMixer()
{
    WorldScene::Get()->OnPlayerBattleStarted += [] 
    {
        engine::SoundManager::Get()->PlaySound(START_BATTLE_SOUND);
    };

    WorldScene::Get()->OnPlayerBattleEnded += [] 
    {
        auto sound = COIN_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*sound);
    };

    group::HumanMind::Get()->OnMarketTransaction += [] 
    {
        auto sound = COIN_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*sound);
    };

    group::HumanMind::Get()->OnItemSold += [] 
    {
        auto sound = COIN_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*sound);
    };

    group::HumanMind::Get()->OnMoneyLooted += [] 
    {
        auto sound = COIN_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*sound);
    };

    group::HumanMind::Get()->OnBribePaid += [] 
    {
        auto sound = COIN_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*sound);
    };

    polity::HumanMind::Get()->OnProductionDecided += [] 
    {
        auto sound = BUILDING_SOUNDS.GetRandom();
        engine::SoundManager::Get()->PlaySound(*sound);
    };
}