#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/WorldDecisionMenu.h"

void WorldDecisionMenu::HandleConfigure() 
{
    searchLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 20.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "Press [S] to search for enemies."}
    );
    searchLabel->Enable();

    restLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -20.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "Press [R] to rest."}
    );
    restLabel->Enable();
}