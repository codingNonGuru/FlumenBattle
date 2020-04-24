#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/Transform.hpp"

#include "FlumenBattle/WorldDecisionMenu.h"

void WorldDecisionMenu::HandleConfigure() 
{
    mainLabel = new Text({"JSLAncient", "Large"}, "MainLabel", Color::RED * 0.5f);
    mainLabel->Configure(Size(150, 150), drawOrder_ + 1, new Transform(Position2(0.0f, 0.0f)), nullptr);

    mainLabel->Enable();
    mainLabel->SetParent(this);

    mainLabel->Setup("Press [G] to start battle.");
}