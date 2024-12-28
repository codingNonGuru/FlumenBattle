#pragma once

#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/World/Character/Item.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/Group/GroupActionData.h"

namespace world
{
    struct WorldTile;

    namespace settlement
    {
        class Settlement;
    }

    namespace character
    {
        class Character;
    }

    namespace polity
    {
        class Polity;
    }
}

namespace world::group
{
    class GroupExtraData;
    class Encounter;
    struct GroupType;
    struct GroupAction;
    class GroupMind;
    union GroupActionData;
    struct GroupActionResult;
    class GroupCore;
    struct SkillData;

    class GroupExtraData
    {
        friend class GroupCore;

        friend class GroupAllocator;

        GroupCore *groupCore;

        const GroupMind *controller;

        Attitudes attitude;

        Pool <character::Character> characters;

        character::ItemManager items;

        character::Character *leader;

        int muleCount;

        polity::Polity *domain;

        void Initialize();

    public:
        void Initialize(GroupCore *);

        bool IsAlive();

        int GetLivingCount() const;

        Pool <character::Character> &GetCharacters() {return characters;}

        character::Character *GetCharacter(int);

        character::Character *GetLeader() const {return leader;}

        SkillData GetMostSkilledMember(character::SkillTypes) const;

        SkillData GetLeastSkilledMember(character::SkillTypes) const;

        int GetCarryCapacity() const;

        int GetCarriedWeight() const;

        bool IsEncumbered() const;

        void Update();

        void CheckFatigue();

        void FinishLongRest();

        void DetermineAction();

        void AddItem(character::ItemTypes, int = 1);

        void RemoveItem(character::Item *);

        void RemoveItemAmount(character::ItemTypes, int);

        world::character::Item *GetItem(int);

        int GetItemAmount(character::ItemTypes);

        const container::Pool <character::Item> &GetItems() const;

        int GetMuleCount() const {return muleCount;}

        void SetMuleCount(int);

        int GetFoodConsumption() const;

        bool DoesRulePolity() const;

        polity::Polity *GetDomain() {return domain;}

        void SetDomain(polity::Polity *);

        int GetDomainSettlementCount() const;

        int GetSize() const;

        void GainExperience(int);

        int GetLevel() const;

        Attitudes GroupExtraData::GetAttitude() const;

        void SetAttitude(Attitudes newAttitude) {attitude = newAttitude;}

        void HandleActionSelection(const GroupActionResult &);

        void HandleActionPerformance(const GroupActionResult &);
    };
}