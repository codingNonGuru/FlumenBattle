#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/WorldInterface.h"
#include "FlumenBattle/WorldDecisionMenu.h"
#include "FlumenBattle/WorldInfoPanel.h"

WorldInterface::WorldInterface()
{
    canvas = ElementFactory::BuildCanvas();

    decisionMenu = ElementFactory::BuildElement<WorldDecisionMenu>(
        {Size(480, 160), DrawOrder(3), {Position2(0.0f, 450.0f), canvas}, {"Sprite"}, Opacity(0.85f)}
    );
    decisionMenu->Enable();

    infoPanel = ElementFactory::BuildElement<WorldInfoPanel>(
        {Size(1900, 100), DrawOrder(3), {Position2(0.0f, -480.0f), canvas}, {"Sprite"}, Opacity(0.85f)}
    );
    infoPanel->Enable();
}

void WorldInterface::Enable()
{
    canvas->Enable();
}

void WorldInterface::Disable()
{
    canvas->Disable();
}