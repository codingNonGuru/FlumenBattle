#pragma once

class ArtificialController
{
    friend class BattleController;
    
    void UpdateCharacter();

public:
    static ArtificialController& Get()
    {
        static ArtificialController controller;

        return controller;
    }
};