#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldTile.h"

namespace world::group
{
    #define BASE_ATTACK_DC 17

    void Encounter::Initialize(Group * first_, Group * second_)
    {
        first = first_; 
        second = second_;

        isOngoing = true;

        isBattle = false;

        hasBattleEnded = false;

        first->EngageGroup(this);
        second->EngageGroup(this);
        
        winner = nullptr;

        static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        isPlayerInvolved = playerGroup == first || playerGroup == second;

        location = first->GetTile();
    }

    void Encounter::Update()
    {
        if(isPlayerInvolved == true)
            return;

        if(isOngoing == false)
            return;

        auto firstStrength = first->GetLivingCount();
        auto secondStrength = second->GetLivingCount();

        if(location->HasRelief(WorldReliefs::MOUNTAINS) == true)
        {
            firstStrength += first->GetHome()->GetModifier(settlement::Modifiers::COMBAT_BONUS_ON_MOUNTAIN_TILES);
            secondStrength += second->GetHome()->GetModifier(settlement::Modifiers::COMBAT_BONUS_ON_MOUNTAIN_TILES);
        }
        else if(location->HasBiome(WorldBiomes::WOODS) == true)
        {
            firstStrength += first->GetHome()->GetModifier(settlement::Modifiers::COMBAT_BONUS_ON_WOOD_TILES);
            secondStrength += second->GetHome()->GetModifier(settlement::Modifiers::COMBAT_BONUS_ON_WOOD_TILES);
        }

        auto firstAttack = utility::RollD20Dice(BASE_ATTACK_DC, firstStrength);
        if(firstAttack.IsAnySuccess() == true)
        {
            for(auto &character : second->GetCharacters())
            {
                if(character.IsAlive() == false)
                    continue;

                auto damage = utility::RollD4Dice();
                character.SufferDamage(damage);
            }
        }

        auto secondAttack = utility::RollD20Dice(BASE_ATTACK_DC, secondStrength);
        if(secondAttack.IsAnySuccess() == true)
        {
            for(auto &character : first->GetCharacters())
            {
                if(character.IsAlive() == false)
                    continue;

                auto damage = utility::RollD4Dice();
                character.SufferDamage(damage);
            }
        }

        firstStrength = first->GetLivingCount();
        secondStrength = second->GetLivingCount();

        if(firstStrength == 0)
        {
            Finish(second);
        }
        else if(secondStrength == 0)
        {
            Finish(first);
        }
    }

    void Encounter::Finish(Group *winner) 
    {
        isOngoing = false;

        this->winner = winner;

        auto loser = GetOtherThan(winner);
        
        this->winner->AddMoney(loser->GetMoney());
    }

    void Encounter::Terminate()
    {
        if(isPlayerInvolved == true)
            return;

        first->ExitBattle();

        second->ExitBattle();
    }
}