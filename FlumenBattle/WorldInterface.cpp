#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/WorldDecisionMenu.h"
#include "FlumenBattle/WorldInfoPanel.h"

WorldInterface::WorldInterface()
{
    decisionMenu = new WorldDecisionMenu();
    decisionMenu->Configure(Size(420, 240), DrawOrder(3), Position2(0.0f, 0.0f), {"Sprite"}, Opacity(0.85f));

    infoPanel = new WorldInfoPanel();
    infoPanel->Configure(Size(1900, 100), DrawOrder(3), Position2(0.0f, -480.0f), {"Sprite"}, Opacity(0.85f));
}

void WorldInterface::Enable()
{
    decisionMenu->Enable();

    infoPanel->Enable();
}

void WorldInterface::Disable()
{
    decisionMenu->Disable();

    infoPanel->Disable();
}