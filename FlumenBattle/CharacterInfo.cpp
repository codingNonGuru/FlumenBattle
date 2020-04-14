#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/TextManager.hpp"
#include "FlumenEngine/Interface/Font.hpp"
#include "FlumenEngine/Interface/FontManager.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/Group.h"

void CharacterInfo::HandleConfigure() 
{
    battleScene = BattleScene::Get();

    auto fontSmall = FontManager::GetFont("DominicanSmall");
    auto fontMedium = FontManager::GetFont("DominicanMedium");
    auto fontVerySmall = FontManager::GetFont("DominicanVerySmall");

    auto textColor = Color::BLACK;

	textLabel = new Text(fontMedium, character->group->GetColor());
    Interface::AddElement("CharacterLabel", textLabel);

	textLabel->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(-5.0f, 0.0f)), nullptr);

	textLabel->Enable();
	textLabel->SetParent(this);

    selectLabel = new Text(fontSmall, textColor);
    Interface::AddElement("SelectLabel", selectLabel);

	selectLabel->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(0.0f, 30.0f)), nullptr);

	selectLabel->SetParent(this);

    selectLabel->Setup("S");

    hitpointLabel = new Text(fontSmall, textColor);
    Interface::AddElement("HitpointLabel", hitpointLabel);

	hitpointLabel->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(20.0f, 0.0f)), nullptr);

	hitpointLabel->SetParent(this);
    hitpointLabel->Enable();

    deathSavingLabel = new Text(fontVerySmall, textColor);
    Interface::AddElement("DeathSavingLabel", deathSavingLabel);

	deathSavingLabel->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(0.0f, 30.0f)), nullptr);

	deathSavingLabel->SetParent(this);

    switch(character->type)
    {
    case CharacterClasses::FIGHTER:
        textLabel->Setup("F");
        break;
    case CharacterClasses::RANGER:
        textLabel->Setup("R");
        break;
    case CharacterClasses::CLERIC:
        textLabel->Setup("C");
        break;
    case CharacterClasses::WIZARD:
        textLabel->Setup("W");
        break;
    }

    GetLeftClickEvents().Add(this, &CharacterInfo::HandleSelection);

    GetRightClickEvents().Add(this, &CharacterInfo::HandleTargeting);

    camera = RenderManager::GetCamera(Cameras::BATTLE);
}

void CharacterInfo::HandleSelection()
{
    battleScene->SelectCharacter(character);
}

void CharacterInfo::HandleTargeting()
{
    battleScene->TargetCharacter(character);
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
    auto string = Word();
	sprintf(string.GetEnd(), "%d", character->currentHitPoints);
    hitpointLabel->Setup(string);
    
    auto position = camera->GetScreenPosition(Position3(character->GetPosition(), 0.0f));

    transform_->GetPosition().x = position.x;
    transform_->GetPosition().y = position.y;

    if(character->IsAlive() == false)
    {
        SetOpacity(0.5f);

        textLabel->SetOpacity(0.5f);

        hitpointLabel->Disable();

        deathSavingLabel->Enable();

        deathSavingLabel->SetOpacity(0.8f);

        Word text;

        if(character->deathThrowFailureCount < 3 && character->deathThrowSuccesCount < 3)
        {
            text << character->deathThrowSuccesCount << "/" << character->deathThrowFailureCount;
        }
        else
        {
            if(character->deathThrowFailureCount >= 3)
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