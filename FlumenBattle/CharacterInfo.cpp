#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/TextManager.hpp"
#include "FlumenEngine/Interface/Font.hpp"
#include "FlumenEngine/Interface/FontManager.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Core/SceneManager.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/Group.h"

void CharacterInfo::HandleConfigure() 
{
    battleScene = SceneManager::Get(Scenes::BATTLE);

    auto fontSmall = FontManager::GetFont("DominicanSmall");
    auto fontMedium = FontManager::GetFont("DominicanMedium");
    auto fontLarge = FontManager::GetFont("DominicanLarge");

    auto textColor = Color::BLACK;

	textLabel = new Text(fontLarge, character->group->GetColor());
    Interface::AddElement("CharacterLabel", textLabel);

	textLabel->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(-10.0f, 0.0f)), nullptr);

	textLabel->Enable();
	textLabel->SetParent(this);

    selectLabel = new Text(fontSmall, textColor);
    Interface::AddElement("SelectLabel", selectLabel);

	selectLabel->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(0.0f, 60.0f)), nullptr);

	selectLabel->SetParent(this);

    selectLabel->Setup("S");

    hitpointLabel = new Text(fontMedium, textColor);
    Interface::AddElement("HitpointLabel", hitpointLabel);

	hitpointLabel->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(40.0f, 0.0f)), nullptr);

	hitpointLabel->SetParent(this);
    hitpointLabel->Enable();

    switch(character->type)
    {
    case CharacterTypes::FIGHTER:
        textLabel->Setup("F");
        break;
    case CharacterTypes::RANGER:
        textLabel->Setup("R");
        break;
    case CharacterTypes::CLERIC:
        textLabel->Setup("C");
        break;
    case CharacterTypes::WIZARD:
        textLabel->Setup("W");
        break;
    }

    //transform_->GetPosition() = Position3(0.0f);//Position3(character->position, 0.0f);

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
	sprintf(string.GetEnd(), "%d", character->hitPointCount);
    hitpointLabel->Setup(string);
    
    //auto position = camera->GetScreenPosition(Position3(character->GetPosition(), 0.0f));
    auto position = camera->GetScreenPosition(Position3(character->GetPosition() * 60.0f, 0.0f));

    transform_->GetPosition().x = position.x;
    transform_->GetPosition().y = position.y;
}