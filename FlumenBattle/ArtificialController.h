#pragma once

struct CombatantData;
struct BattleTile;

enum class CombatSearchFilterTypes
{
    IS_ALLY, IS_ENEMY, IS_VULNERABLE, IS_WITHIN_RANGE, IS_ALIVE
};

enum class CombatantSinglingCriteria
{
    IS_CLOSEST, IS_MOST_VULNERABLE
};

struct CombatSearchFilter
{
    CombatSearchFilterTypes Type;

    Integer Value;

    bool operator== (const CombatSearchFilter &filter) {return Type == filter.Type;}

    CombatSearchFilter() {}

    CombatSearchFilter(CombatSearchFilterTypes type) : Type(type) {}

    CombatSearchFilter(CombatSearchFilterTypes type, Integer value) : Type(type), Value(value) {}
};

typedef std::initializer_list <CombatSearchFilter> FilterList;

class ArtificialController
{
    friend class BattleController;

    BattleTile * FindClosestFreeTile(BattleTile *, BattleTile *);

    CombatantData FindCombatant(FilterList, CombatantSinglingCriteria);

    void DetermineActionCourse();

    void DetermineFighterBehavior();

    void DetermineRangerBehavior();

    void PerformAction();

    void MoveCharacter();

    void UpdateCharacter();

    bool CheckOpportunity();

public:
    static ArtificialController& Get()
    {
        static ArtificialController controller;

        return controller;
    }
};