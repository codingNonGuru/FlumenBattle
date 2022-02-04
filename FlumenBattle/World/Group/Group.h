#pragma once

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Types.h"
#include "FlumenBattle/World/Group/GroupActionData.h"

class Character;

enum class RaceTypes;
class Character;
class CombatGroup;
namespace world
{
    class Battle;
    class WorldScene;
    class WorldTile;
    class WorldDecisionMenu;
}

namespace world::group
{
    struct GroupType;
    struct GroupAction;
    class GroupController;
    class GroupFactory;
    class GroupActionPerformer;
    class GroupActionValidator;

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

        Word name;

        const GroupType *type;

        const GroupController *controller;

        const GroupAction *action;

        Integer actionProgress;

        SuccessTypes actionSuccess;

        ActionIntensities actionIntensity;

        Battle *battle;

        WorldTile *tile;

        WorldTile *destination;

        Color color;

        Pool <Character> characters;

        void Initialize(Word, const GroupType *, Integer, Color, RaceTypes);

    public:
        Group();
        
        bool IsAlive();

        const Color& GetColor() const {return color;} 

        Word GetName() const {return name;}

        Battle * GetBattle() const {return battle;}

        WorldTile * GetTile() const {return tile;}

        const GroupAction * GetAction() const {return action;}

        SuccessTypes GetActionSuccess() const {return actionSuccess;}

        Pool <Character> & GetCharacters() {return characters;}

        void DetermineAction();

        void PerformAction();

        bool ValidateAction(GroupActions, const GroupActionData & = GroupActionData());

        GroupActionResult SelectAction(GroupActions, const GroupActionData & = GroupActionData());

        void CancelAction();

        void IntensifyAction();

        void SlackenAction();

        void EnterBattle(Battle *);

        void ExitBattle();

        WorldTile *GetDestination() const {return destination;}

        void SetDestination(WorldTile *destination) {this->destination = destination;}

        void SetTile(WorldTile *tile);

        float GetActionProgress() const;

        int GetRemainingActionDuration() const;

        int GetProgressRate() const;
    };
}