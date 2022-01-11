#pragma once

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Types.h"

class Character;

enum class RaceTypes;
class Character;
class CombatGroup;
namespace world
{
    class Battle;
    class WorldScene;
    class WorldTile;
}

namespace world::group
{
    struct GroupType;
    struct GroupAction;
    class GroupController;
    class GroupFactory;
    class GroupActionPerformer;

    class Group
    {
        friend class container::Pool <Group>;

        friend class GroupFactory;

        friend class world::WorldScene;

        friend class Character;

        friend class CombatGroup;

        friend class GroupActionPerformer;

        Word name;

        const GroupType *type;

        const GroupController *controller;

        const GroupAction *action;

        Integer actionProgress;

        Battle *battle;

        WorldTile *tile;

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

        Pool <Character> & GetCharacters() {return characters;}

        void DetermineAction();

        void PerformAction();

        bool ValidateAction(world::GroupActions);

        void SelectAction(world::GroupActions);

        void CancelAction();

        void EnterBattle(Battle *);

        void ExitBattle();
    };
}