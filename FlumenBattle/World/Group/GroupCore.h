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

    struct CharacterEssence
    {
        bool isFunctioning;

        bool isAlive;

        character::CharacterClasses characterClass;

        int experience;

        int level;

        int health;
    };

    struct CharacterHandler
    {
        int characterCount;

        container::SmartBlock <CharacterEssence, 16> *characters;        
    };

    struct SkillData
    {
        character::Character *Character;

        int Bonus;
    };

    struct TravelActionData
    {
        WorldTile *Source {nullptr};

        WorldTile *Destination {nullptr};

        Integer Progress {0};

        Integer ProgressSinceCheck {0};

        Integer Duration {0};

        bool IsLost {false};

        WorldTile *Route[TILES_PER_GROUP_ROUTE];

        bool IsOnRoute {false};

        int PlannedDestinationCount {0};

        ActionIntensities Intensity {ActionIntensities::NORMAL};
    }; 

    class GroupCore
    {
        friend class GroupExtraData;

        struct GroupLocation
        {
            friend class GroupCore;

        private:
            WorldTile *Tile;

            GroupLocation &operator =(WorldTile *tile) {Tile = tile;}

        public:
            operator WorldTile *() {return Tile;}

            WorldTile *operator ->() {return Tile;}
        };

        bool isAlive;

        int uniqueId;

        int level;

        const GroupType *type;

        RaceTypes raceType;

        const GroupAction *action;

        int actionProgress;

        settlement::Settlement *home;

        WorldTile *tile;

        Encounter *encounter;

        int timeSinceLongRest;

        bool isFatigued;

        int conditionFactor;

        int conditionTimestamp;

        int money;

        int itemValue;

        bool hasAchievedObjective {false};

        bool hasMission;

        bool hasAttemptedPersuasion;

        bool hasAttemptedBypassingDefences;

        bool hasAttemptedBribingGarrison;

        CharacterHandler characterHandler;

        TravelActionData *travelActionData;

        GroupExtraData *extraData;

    public:
        int GetUniqueId() const {return uniqueId;}

        bool HasUniqueId(int id) const {return uniqueId == id;}
        
        bool IsAlive();

        Encounter *GetEncounter() const {return encounter;}

        WorldTile *GetTile() const {return tile;}

        settlement::Settlement *GetHome() const {return home;}

        Position2 GetVisualPosition() const;

        const GroupAction *GetAction() const {return action;}

        bool IsDoing(GroupActions) const;

        bool IsDoingSomething() const;

        bool IsInEncounter() const;

        Pool <character::Character> &GetCharacters();

        int GetLivingCount() const;

        character::Character *GetCharacter(int);

        character::Character *GetLeader() const;

        SkillData GetMostSkilledMember(character::SkillTypes) const;

        SkillData GetLeastSkilledMember(character::SkillTypes) const;

        GroupCore *GetOther();

        GroupClasses GetClass() const;

        int GetCarryCapacity() const;

        int GetCarriedWeight() const;

        WorldTile *GetDestination() const {return travelActionData->Destination;}

        WorldTile *GetFinalDestination() const;

        WorldTile *GetTravelStartPoint() const {return travelActionData->Source;}

        float GetTravelProgress() const;

        bool IsEncumbered() const;

        void Update();

        void CheckFatigue();

        void DetermineAction();

        void PerformAction();

        bool ValidateAction(GroupActions, const GroupActionData & = GroupActionData());

        GroupActionResult SelectAction(GroupActions, const GroupActionData & = GroupActionData());

        void CancelAction();

        void IntensifyAction();

        void SlackenAction();

        void EngageGroup(Encounter *);

        void ExitBattle();

        void AddItem(character::ItemTypes, int = 1);

        void RemoveItem(character::Item *);

        void RemoveItemAmount(character::ItemTypes, int);

        void SetTile(WorldTile *tile);

        void SetHome(settlement::Settlement *newHome) {home = newHome;}

        float GetActionProgress() const;

        int GetRemainingActionDuration() const;

        int GetProgressRate() const;

        world::character::Item *GetItem(int);

        int GetItemAmount(character::ItemTypes);

        const container::Pool <character::Item> &GetItems() const;

        const int &GetMoney() const {return money;}

        settlement::Settlement *GetCurrentSettlement();

        int GetDistanceTo(const GroupCore *) const;

        int GetMuleCount() const;

        void AddMoney(int sum) {money += sum;}

        int GetFoodConsumption() const;

        bool DoesRulePolity() const;

        polity::Polity *GetDomain();

        void SetDomain(polity::Polity *);

        int GetDomainSettlementCount() const;

        bool HasSameAllegiance(const GroupCore &) const;

        bool HasAttemptedBypassingDefences() const {return hasAttemptedBypassingDefences;}

        bool HasAttemptedBribingGarrison() const {return hasAttemptedBribingGarrison;}

        int GetSize() const;

        void GainExperience(int);

        int GetLevel() const;
    };

    class GroupExtraData
    {
        friend class GroupCore;

        GroupCore *groupCore;

        const GroupMind *controller;

        bool isAlive;

        Attitudes attitude;

        Pool <character::Character> characters;

        character::ItemManager items;

        character::Character *leader;

        int muleCount;

        polity::Polity *domain;

        void Initialize(const GroupType *, Integer, RaceTypes);

        void *operator new(size_t);

    public:
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

        void DetermineAction();

        void AddItem(character::ItemTypes, int = 1);

        void RemoveItem(character::Item *);

        void RemoveItemAmount(character::ItemTypes, int);

        world::character::Item *GetItem(int);

        int GetItemAmount(character::ItemTypes);

        const container::Pool <character::Item> &GetItems() const;

        int GetMuleCount() const {return muleCount;}

        int GetFoodConsumption() const;

        bool DoesRulePolity() const;

        polity::Polity *GetDomain() {return domain;}

        void SetDomain(polity::Polity *);

        int GetDomainSettlementCount() const;

        int GetSize() const;

        void GainExperience(int);

        int GetLevel() const;
    };
}