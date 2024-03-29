#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/Battle/BattleEndMessage.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/CombatGroup.h"

using namespace battle;

void BattleEndMessage::HandleConfigure() 
{
    mainLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 0.0f), this}},
        {{"Large"}, Color::RED * 0.5f}
    );
    mainLabel->Enable();

    BattleController::Get()->OnBattleEnded += {this, &BattleEndMessage::Enable};
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

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_J, {this, &BattleEndMessage::HandleConfirm});
}

void BattleEndMessage::HandleConfirm()
{
    BattleController::Get()->ExitBattle();

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_J);
}