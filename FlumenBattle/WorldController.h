#pragma once

#include "FlumenEngine/Core/Singleton.h"

class WorldController : public Singleton<WorldController>
{
    void EnableInput();

    void DisableInput();

    void HandleSPressed();  

    void HandleRPressed();  

    void StartBattle();

    void Rest();

public:
    void Enable();

    void Disable();
};