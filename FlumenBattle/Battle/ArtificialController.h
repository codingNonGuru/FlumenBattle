#pragma once

#include "FlumenBattle/Types.hpp"

namespace battle
{
    struct CombatantData;
    struct BattleTile;
    struct CombatSearchFilter;
    enum class SinglingCriteria;

    typedef std::initializer_list <CombatSearchFilter> FilterList;

    class ArtificialController
    {
        friend class BattleController;

        CombatantData FindCombatant(FilterList, SinglingCriteria);

        bool ApproachTile(BattleTile *, Integer);

        BattleTile * GetVirtualTile();

        void RepeatActionCycle();

        void DetermineActionCourse();

        void DetermineFighterBehavior();

        void DetermineRangerBehavior();

        void DetermineClericBehavior();

        void DetermineWizardBehavior();

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
}