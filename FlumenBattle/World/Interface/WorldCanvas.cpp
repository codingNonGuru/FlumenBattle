#include "FlumenEngine/Interface/Scroller.h"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "WorldCanvas.h"
#include "FlumenBattle/World/WorldController.h"

using namespace world::interface;

void WorldCanvas::HandleConfigure()
{
    MakeScrollable(1);

    GetScrollUpEvents() += {this, &WorldCanvas::HandleScrollUp};

    GetScrollDownEvents() += {this, &WorldCanvas::HandleScrollDown};
}

void WorldCanvas::HandleScrollUp()
{
    WorldController::Get()->ZoomIn();
}

void WorldCanvas::HandleScrollDown()
{
    WorldController::Get()->ZoomOut();
}