#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/WorldDecisionMenu.h"

void WorldDecisionMenu::HandleConfigure() 
{
    travelLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 30.0f), this}},
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f, "Press [T] to travel to location."}
    );
    travelLabel->Enable();

    searchLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 0.0f), this}},
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f, "Press [S] to search for enemies."}
    );
    searchLabel->Enable();

    restLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -30.0f), this}},
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f, "Press [R] to rest."}
    );
    restLabel->Enable();
}