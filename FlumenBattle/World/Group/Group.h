#pragma once

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Types.h"
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

    namespace character
    {
        class Character;
        class Item;
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

        Word name;

        const GroupType *type;

        const GroupMind *controller;

        const GroupAction *action;

        struct TravelActionData
        {
            WorldTile *Source {nullptr};

            WorldTile *Destination {nullptr};

            Integer Progress {0};

            Integer ProgressSinceCheck {0};

            bool IsLost {false};

            ActionIntensities Intensity {ActionIntensities::NORMAL};
        } travelActionData;

        Integer actionProgress;

        utility::Success actionSuccess;

        int timeSinceLongRest;

        Encounter *encounter;

        Attitudes attitude;

        bool hasAttemptedPersuasion;

        WorldTile *tile;

        Color color;

        Pool <character::Character> characters;

        character::ItemManager items;

        character::Character *leader;

        void Initialize(Word, const GroupType *, Integer, Color, RaceTypes);

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

        void SetTile(WorldTile *tile);

        float GetActionProgress() const;

        int GetRemainingActionDuration() const;

        int GetProgressRate() const;

        world::character::Item *GetItem(int);
    };
}