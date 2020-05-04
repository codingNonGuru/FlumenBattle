#pragma once

#include "FlumenEngine/Core/Singleton.h"

class WorldController : public Singleton<WorldController>
{
    void HandleActionPerformed();

    void StartBattle();

    void HandleSpacePressed();

    void HandleSpeedUpTime();

    void HandleSlowDownTime();

public:
    void Enable();

    void Disable();
};