#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/Transform.hpp"

#include "FlumenBattle/BattleEndMessage.h"
#include "FlumenBattle/BattleController.h"

void BattleEndMessage::HandleConfigure() 
{
    mainLabel = new Text({"JSLAncient", "Large"}, "BattleEndLabel", Color::RED);
    mainLabel->Configure(Size(150, 150), drawOrder_ + 1, new Transform(Position2(0.0f, 0.0f)), nullptr);

    mainLabel->Enable();
    mainLabel->SetParent(this);

    mainLabel->Setup("Bye");

    BattleController::Get()->OnBattleEnded.Add(this, &BattleEndMessage::Enable);
}

void BattleEndMessage::HandleEnable() 
{

}

void BattleEndMessage::HandleDisable() 
{

}