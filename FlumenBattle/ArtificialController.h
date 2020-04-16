#pragma once

class ArtificialController
{
    friend class BattleController;

    void DetermineActionCourse();

    void MoveCharacter();

    void UpdateCharacter();

public:
    static ArtificialController& Get()
    {
        static ArtificialController controller;

        return controller;
    }
};