#pragma once

struct CombatantData;

class ArtificialController
{
    friend class BattleController;

    CombatantData FindClosestEnemy();

    CombatantData FindClosestAlly();

    void DetermineActionCourse();

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