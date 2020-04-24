#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/FontManager.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/Combatant.h"
#include "FlumenBattle/CombatGroup.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/HumanController.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/CharacterClass.h"

void CharacterInfo::HandleConfigure() 
{
    battleController = BattleController::Get();

    /*auto fontSmall = FontManager::GetFont("DominicanSmall");
    auto fontMedium = FontManager::GetFont("DominicanMedium");
    auto fontVerySmall = FontManager::GetFont("DominicanVerySmall");*/

    auto textColor = Color::BLACK;

	textLabel = new Text({"JSLAncient", "Medium"}, textColor);
	textLabel->Configure(Size(150, 150), DrawOrder(2), Position2(-5.0f, 0.0f));

	textLabel->Enable();
	textLabel->SetParent(this);

    selectLabel = new Text({"JSLAncient", "Small"}, textColor);
	selectLabel->Configure(Size(150, 150), DrawOrder(2), Position2(0.0f, 30.0f));

	selectLabel->SetParent(this);

    selectLabel->Setup("S");

    targetedLabel = new Text({"JSLAncient", "Small"}, textColor);
	targetedLabel->Configure(Size(150, 150), DrawOrder(2), Position2(0.0f, -30.0f));

	targetedLabel->SetParent(this);

    targetedLabel->Setup("x");

    hitpointLabel = new Text({"JSLAncient", "Small"}, textColor);
	hitpointLabel->Configure(Size(150, 150), DrawOrder(2), Position2(20.0f, 0.0f));

	hitpointLabel->SetParent(this);

    deathSavingLabel = new Text({"JSLAncient", "VerySmall"}, textColor);
	deathSavingLabel->Configure(Size(150, 150), DrawOrder(2), Position2(0.0f, 30.0f));

	deathSavingLabel->SetParent(this);

    GetLeftClickEvents().Add(this, &CharacterInfo::HandleTargeting);
}

void CharacterInfo::HandleEnable() 
{
    camera = RenderManager::GetCamera(Cameras::BATTLE);
    
    auto className = combatant->character->type->Name;
    textLabel->Setup(className.GetFirstCharacter());

    textLabel->SetColor(combatant->GetGroup()->GetGroup()->GetColor());

    SetOpacity(1.0f);

    textLabel->SetOpacity(1.0f);

    hitpointLabel->Enable();

    deathSavingLabel->Disable();
}

void CharacterInfo::HandleTargeting()
{
    HumanController::Get()->TargetCombatant(combatant);
}

void CharacterInfo::Select()
{
    selectLabel->Enable();
}

void CharacterInfo::Deselect()
{
    selectLabel->Disable();
}

void CharacterInfo::HandleUpdate()
{
    auto string = Word() << combatant->character->currentHitPoints;
    hitpointLabel->Setup(string);

    if(battleController->GetTargetedCombatant() == combatant)
    {
        targetedLabel->Enable();
    }
    else
    {
        targetedLabel->Disable();
    }
    
    auto position = camera->GetScreenPosition(Position3(combatant->GetPosition(), 0.0f));

    transform_->GetPosition().x = position.x;
    transform_->GetPosition().y = position.y;

    if(combatant->IsAlive() == false)
    {
        SetOpacity(0.5f);

        textLabel->SetOpacity(0.5f);

        hitpointLabel->Disable();

        deathSavingLabel->Enable();

        deathSavingLabel->SetOpacity(0.8f);

        Word text;

        if(combatant->deathThrowFailureCount < 3 && combatant->deathThrowSuccesCount < 3)
        {
            text << combatant->deathThrowSuccesCount << "/" << combatant->deathThrowFailureCount;
        }
        else
        {
            if(combatant->deathThrowFailureCount >= 3)
            {
                text << "Dead";
            }
            else
            {
                text << "Stable";
            }
        }

        deathSavingLabel->Setup(text);
    }
}

void CharacterInfo::SetCombatant(Combatant *_combatant) 
{
    combatant = _combatant;
    combatant->info = this;
}