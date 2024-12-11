#include "FlumenBattle/World/Group/Encounter.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/WorldTile.h"

namespace world::group
{
    #define BASE_ATTACK_DC 17

    void Encounter::Initialize(Group * attacker_, Group * defender_)
    {
        attacker = attacker_; 
        defender = defender_;

        isOngoing = true;

        isBattle = false;

        hasBattleEnded = false;

        attacker->EngageGroup(this);
        defender->EngageGroup(this);
        
        winner = nullptr;

        static const auto playerGroup = WorldScene::Get()->GetPlayerGroup();

        isPlayerInvolved = playerGroup == attacker || playerGroup == defender;

        location = attacker->GetTile();

        if(location->GetSettlement() != nullptr)
        {
            isSiege = true;
        }
        else
        {
            isSiege = false;
        }
    }

    void Encounter::SetSiege(bool isSiege)
    {
        this->isSiege = isSiege;
    }

    void Encounter::Update()
    {
        if(isPlayerInvolved == true)
            return;

        if(isOngoing == false)
            return;

        auto firstStrength = attacker->GetLivingCount();
        auto secondStrength = defender->GetLivingCount();

        if(location->HasRelief(WorldReliefs::MOUNTAINS) == true)
        {
            firstStrength += attacker->GetHome()->GetModifier(settlement::Modifiers::COMBAT_BONUS_ON_MOUNTAIN_TILES);
            secondStrength += defender->GetHome()->GetModifier(settlement::Modifiers::COMBAT_BONUS_ON_MOUNTAIN_TILES);
        }
        else if(location->HasBiome(WorldBiomes::WOODS) == true)
        {
            firstStrength += attacker->GetHome()->GetModifier(settlement::Modifiers::COMBAT_BONUS_ON_WOOD_TILES);
            secondStrength += defender->GetHome()->GetModifier(settlement::Modifiers::COMBAT_BONUS_ON_WOOD_TILES);
        }

        auto firstAttack = utility::RollD20Dice(BASE_ATTACK_DC, firstStrength);
        if(firstAttack.IsAnySuccess() == true)
        {
            for(auto &character : defender->GetCharacters())
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
            for(auto &character : attacker->GetCharacters())
            {
                if(character.IsAlive() == false)
                    continue;

                auto damage = utility::RollD4Dice();
                character.SufferDamage(damage);
            }
        }

        firstStrength = attacker->GetLivingCount();
        secondStrength = defender->GetLivingCount();

        if(firstStrength == 0)
        {
            Finish(defender);
        }
        else if(secondStrength == 0)
        {
            Finish(attacker);
        }
    }

    void Encounter::Finish(Group *winner) 
    {
        isOngoing = false;

        this->winner = winner;
    }

    void Encounter::Terminate()
    {
        if(isPlayerInvolved == true)
            return;

        attacker->ExitBattle();

        defender->ExitBattle();
    }
}