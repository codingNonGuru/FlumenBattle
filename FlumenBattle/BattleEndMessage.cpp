#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/BattleEndMessage.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/CombatGroup.h"

void BattleEndMessage::HandleConfigure() 
{
    mainLabel = new Text({"JSLAncient", "Large"}, "BattleEndLabel", Color::RED);
    mainLabel->Configure(Size(150, 150), drawOrder_ + 1, new Transform(Position2(0.0f, 0.0f)), nullptr);

    mainLabel->Enable();
    mainLabel->SetParent(this);

    //mainLabel->Setup("Bye");

    BattleController::Get()->OnBattleEnded.Add(this, &BattleEndMessage::Enable);
}    

void BattleEndMessage::HandleEnable() 
{
    if(BattleScene::Get()->GetPlayerGroup()->IsAlive())
    {
        mainLabel->Setup("You won. Press [J] to return to the campaign.");
    }
    else
    {
        mainLabel->Setup("You are dead. Game over. Press [J] to exit.");
    }

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_J, this, &BattleEndMessage::HandleConfirm);
}

void BattleEndMessage::HandleConfirm()
{
    BattleController::Get()->ExitBattle();

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_J, this, &BattleEndMessage::HandleConfirm);
}