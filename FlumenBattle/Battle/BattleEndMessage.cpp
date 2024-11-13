#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "FlumenBattle/Battle/BattleEndMessage.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/BattleScene.h"
#include "FlumenBattle/Battle/CombatGroup.h"

using namespace battle;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto BORDER_INNER_OFFSET = Size(4, 4);

void BattleEndMessage::HandleConfigure() 
{
    mainLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {this}},
        {{"Large"}, TEXT_COLOR}
    );
    mainLabel->Enable();

    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - BORDER_INNER_OFFSET, 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true} 
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

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