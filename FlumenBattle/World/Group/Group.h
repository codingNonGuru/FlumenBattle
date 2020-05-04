#pragma once

#include "FlumenEngine/Utility/Color.hpp"

#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Types.h"

class Character;
class Battle;
enum class RaceTypes;
class Character;
class CombatGroup;
namespace world
{
    class WorldScene;
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
        friend class container::Array <Group>;

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

        Color color;

        Array <Character> characters;

        Group();

        void Initialize(Word, const GroupType *, Integer, Color, RaceTypes);

    public:
        bool IsAlive();

        const Color& GetColor() const {return color;} 

        Word GetName() const {return name;}

        Array <Character> & GetCharacters() {return characters;}

        void DetermineAction();

        void PerformAction();

        bool ValidateAction(world::GroupActions);

        void SelectAction(world::GroupActions);

        void CancelAction();
    };
}