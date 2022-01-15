#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "MainMenu.h"

using namespace pregame;

void MainMenu::HandleConfigure() 
{
    startGameLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -20.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "[S]tart Game"}
    );
    startGameLabel->Enable();

    quitGameLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 20.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "[E]xit Game"}
    );
    quitGameLabel->Enable();
}