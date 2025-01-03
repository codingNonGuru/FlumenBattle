#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/ProgressBar.h"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"

#include "FlumenBattle/Battle/CharacterInfo.h"
#include "FlumenBattle/World/Character/Condition.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/CombatGroup.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/HumanController.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/World/Character/CharacterClass.h"

using namespace battle;

static const auto HEALTHBAR_MIN_SIZE = Size(40, 16);

static const auto HEALTHBAR_MAX_SIZE = Size(160, 32);

void CharacterInfo::HandleConfigure() 
{
    battleController = BattleController::Get();

    auto textColor = Color::BLACK;

    selectLabel = ElementFactory::BuildText(
        {Size(150, 150), DrawOrder(2), {Position2(0.0f, 30.0f), this}},
        {{"Small"}, textColor, "S"}
    );

    targetedLabel = ElementFactory::BuildElement <Element>(
        {DrawOrder(2), {this}, {"RedArrow", false}}
    );
    targetedLabel->UpdatePositionConstantly();

    hitpointLabel = ElementFactory::BuildText(
        {Size(150, 150), DrawOrder(4), {this}},
        {{"VerySmall"}, Color::WHITE}
    );

    healthBar = ElementFactory::BuildProgressBar <ProgressBar>(
        {HEALTHBAR_MIN_SIZE, DrawOrder(2), {this}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );
    healthBar->Enable();

    deathSavingLabel = ElementFactory::BuildText(
        {Size(150, 150), DrawOrder(2), {Position2(0.0f, 30.0f), this}},
        {{"VerySmall"}, textColor}
    );

    SetInteractivity(true);

    GetLeftClickEvents() += {this, &CharacterInfo::HandleTargeting};
}

void CharacterInfo::HandleEnable() 
{
    camera = RenderManager::GetCamera(Cameras::BATTLE);
    
    auto className = combatant->character->type->Name;

    SetOpacity(0.0f);

    //hitpointLabel->Enable();

    deathSavingLabel->Disable();
}

void CharacterInfo::HandleTargeting()
{
    HumanController::Get()->TargetCombatant(combatant);
}

void CharacterInfo::HandleHover()
{
    BattleInterface::Get()->EnableHoverExtension(this);
}

void CharacterInfo::Select()
{
    //selectLabel->Enable();
}

void CharacterInfo::Deselect()
{
    //selectLabel->Disable();
}

void CharacterInfo::HandleUpdate()
{
    auto healthRatio = (float)combatant->character->currentHitPoints / (float)combatant->character->maximumHitPoints;
    healthBar->SetProgress(healthRatio);

    auto barOffset = 50.0f / camera->GetZoomFactor();
    healthBar->SetBasePosition({0.0f, barOffset});

    auto scaleFactor = 1.0f - exp(-camera->GetZoomFactor() / 1.5f);
    scaleFactor -= 0.15f;
    scaleFactor *= 1.3f;

    healthBar->SetSize({(float)HEALTHBAR_MIN_SIZE.x * scaleFactor + (float)HEALTHBAR_MAX_SIZE.x * (1.0f - scaleFactor),
        (float)HEALTHBAR_MIN_SIZE.y * scaleFactor + (float)HEALTHBAR_MAX_SIZE.y * (1.0f - scaleFactor)}
    );

    if(scaleFactor < 0.35f)
    {
        auto string = Word() << combatant->character->currentHitPoints << " / " << combatant->character->maximumHitPoints;
        hitpointLabel->Setup(string);

        hitpointLabel->Enable();

        hitpointLabel->SetBasePosition({0.0f, barOffset});
    }
    else
    {
        hitpointLabel->Disable();
    }

    if(battleController->GetTargetedCombatant() == combatant)
    {
        targetedLabel->Enable();

        auto offset = 40.0f / camera->GetZoomFactor();

        targetedLabel->SetBasePosition({0.0f, -offset});

        auto scaleFactor = 1.5f / utility::Clamp(camera->GetZoomFactor(), 0.2f, 2.0f);
        targetedLabel->GetSprite()->SetTextureSize(Scale2(scaleFactor));
    }
    else
    {
        targetedLabel->Disable();
    }
    
    auto position = camera->GetScreenPosition(Position3(combatant->GetPosition(), 0.0f));

    transform_->GetPosition().x = position.x;
    transform_->GetPosition().y = position.y;

    transform_->GetScale() = 0.7f / camera->GetZoomFactor();

    if(combatant->IsAlive() == false)
    {
        //SetOpacity(0.5f);

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