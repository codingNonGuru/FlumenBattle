#pragma once

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Group/Types.h"
#include "FlumenBattle/World/Group/GroupActionData.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/Character/Item.h"

enum class RaceTypes;

namespace battle
{
    class CombatGroup;
}

namespace world
{
    class WorldScene;
    class WorldTile;
    class WorldDecisionMenu;
    class WorldTileModel;

    namespace character
    {
        class Character;
        class Item;
    }

    namespace settlement
    {
        class Settlement;
    }
}

namespace world::group
{
    struct GroupType;
    struct GroupAction;
    class GroupMind;
    class GroupFactory;
    class GroupActionPerformer;
    class GroupActionValidator;
    class Encounter;
    class MachineMind;
    class HumanMind;
    enum class GroupClasses;

    class Group
    {
        friend class container::Pool <Group>;

        friend class GroupFactory;

        friend class world::WorldScene;

        friend class Character;

        friend class CombatGroup;

        friend class GroupActionPerformer;

        friend class GroupActionValidator;

        friend class world::WorldDecisionMenu;

        friend class GroupAllocator;

        friend class MachineMind;

        friend class HumanMind;

        friend class world::WorldTileModel;

        Word name;

        const GroupType *type;

        const GroupMind *controller;

        const GroupAction *action;

        bool isAlive;

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
        } travelActionData;

        Integer actionProgress;

        utility::Success actionSuccess;

        int timeSinceLongRest;

        bool hasAchievedObjective {false};

        Encounter *encounter;

        Attitudes attitude;

        bool hasAttemptedPersuasion;

        WorldTile *tile;

        settlement::Settlement *home;

        Color color;

        Pool <character::Character> characters;

        character::ItemManager items;
        
        int money;

        character::Character *leader;

        void Initialize(Word, const GroupType *, Integer, Color, RaceTypes);

        void *operator new(size_t);

        void operator delete(void *);

    public:
        Group();
        
        bool IsAlive();

        const Color& GetColor() const {return color;} 

        Word GetName() const {return name;}

        Encounter * GetEncounter() const {return encounter;}

        WorldTile * GetTile() const {return tile;}

        const GroupAction * GetAction() const {return action;}

        bool IsDoing(GroupActions actionType) const;

        utility::Success GetActionSuccess() const {return actionSuccess;}

        Pool <character::Character> & GetCharacters() {return characters;}

        character::Character *GetCharacter(int index);

        character::Character *GetLeader() const {return leader;}

        Group *GetOther();

        GroupClasses GetClass() const;

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

        WorldTile *GetDestination() const {return travelActionData.Destination;}

        WorldTile *GetFinalDestination() const;

        WorldTile *GetTravelStartPoint() const {return travelActionData.Source;}

        float GetTravelProgress() const;

        void SetTile(WorldTile *tile);

        void SetHome(settlement::Settlement *newHome) {home = newHome;}

        float GetActionProgress() const;

        int GetRemainingActionDuration() const;

        int GetProgressRate() const;

        world::character::Item *GetItem(int);

        const int &GetMoney() const {return money;}
    };
}