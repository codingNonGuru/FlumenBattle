#pragma once

#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/Race.h"
#include "FlumenBattle/World/Character/Item.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/Group/GroupActionData.h"

namespace world
{
    namespace tile {struct WorldTile;}

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
    struct CharacterEssence;

    typedef container::SmartBlock <CharacterEssence, 16> CharacterEssenceBatch;

    struct CharacterHandler
    {
        int characterCount;

        CharacterEssenceBatch *characters;

        CharacterEssence *AddCharacter();

        void DamageCharacter(CharacterEssence &, int);        
    };

    struct SkillData
    {
        character::Character *Character;

        int Bonus;
    };

    struct TravelActionData
    {
        tile::WorldTile *Source {nullptr};

        tile::WorldTile *Destination {nullptr};

        Integer Progress {0};

        Integer ProgressSinceCheck {0};

        Integer Duration {0};

        bool IsLost {false};

        tile::WorldTile *Route[TILES_PER_GROUP_ROUTE];

        bool IsOnRoute {false};

        int PlannedDestinationCount {0};

        ActionIntensities Intensity {ActionIntensities::NORMAL};
    }; 

    class GroupCore
    {
        friend class GroupExtraData;

        friend class GroupAllocator;

        friend class GroupActionPerformer;

        friend class GroupActionValidator;

        friend class GroupFactory;

        friend class MachineMind;

        friend class HumanMind;

        struct GroupLocation
        {
            friend class GroupCore;

        private:
            tile::WorldTile *Tile;

            GroupLocation &operator =(tile::WorldTile *tile) {Tile = tile;}

        public:
            operator tile::WorldTile *() {return Tile;}

            tile::WorldTile *operator ->() {return Tile;}
        };

        bool isAlive;

        int uniqueId;

        int level;

        const GroupType *type;

        RaceTypes raceType;

        const GroupAction *action;

        int actionProgress;

        settlement::Settlement *home;

        tile::WorldTile *tile;

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
        void Initialize(const GroupType *, Integer, RaceTypes);

        bool IsDeepGroup() const {return extraData != nullptr;}

        int GetUniqueId() const {return uniqueId;}

        bool HasUniqueId(int id) const {return uniqueId == id;}
        
        bool IsAlive();

        Encounter *GetEncounter() const {return encounter;}

        tile::WorldTile *GetTile() const {return tile;}

        settlement::Settlement *GetHome() const {return home;}

        Position2 GetVisualPosition() const;

        const GroupAction *GetAction() const {return action;}

        bool IsDoing(GroupActions) const;

        bool IsDoingSomething() const;

        bool IsInEncounter() const;

        Pool <character::Character> &GetCharacters();

        CharacterEssenceBatch &GetCharacterEssences() {return *characterHandler.characters;}

        int GetLivingCount() const;

        character::Character *GetCharacter(int);

        character::Character *GetLeader() const;

        SkillData GetMostSkilledMember(character::SkillTypes) const;

        SkillData GetLeastSkilledMember(character::SkillTypes) const;

        GroupCore *GetOther();

        GroupClasses GetClass() const;

        int GetCarryCapacity() const;

        int GetCarriedWeight() const;

        tile::WorldTile *GetDestination() const {return travelActionData->Destination;}

        tile::WorldTile *GetFinalDestination() const;

        tile::WorldTile *GetTravelStartPoint() const {return travelActionData->Source;}

        float GetTravelProgress() const;

        bool IsEncumbered() const;

        void Update();

        void CheckFatigue();

        void FinishLongRest();

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

        void SetTile(tile::WorldTile *tile);

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

        void SetMuleCount(int);

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

        Attitudes GetAttitude() const;

        void SetAttitude(Attitudes);

        void AddPositiveCondition(int);

        void AddNegativeCondition(int);

        TravelActionData *GetTravelData() {return travelActionData;}

        CharacterHandler &GetCharacterHandler() {return characterHandler;}

        Word GetLeaderName() const;

        void ChangeType(GroupClasses);
    };
}