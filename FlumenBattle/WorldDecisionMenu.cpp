#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/WorldDecisionMenu.h"

void WorldDecisionMenu::HandleConfigure() 
{
    mainLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 0.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "Press [G] to start battle."}
    );

    mainLabel->Enable();
}