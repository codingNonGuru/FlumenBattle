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

#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/BattleInfoPanel.h"

static BattleScene* battleScene = nullptr;

void BattleInfoPanel::HandleConfigure() 
{
    battleScene = (BattleScene*)SceneManager::Get(Scenes::BATTLE);
    battleScene->OnCharacterActed.Add(this, &HandleCharacterAction);

    auto fontSmall = FontManager::GetFont("DominicanSmall");

    auto textColor = (Color::RED * 0.3f) + (Color::BLACK * 0.7f);

	actionLabel = new Text(fontSmall, textColor);
    Interface::AddElement("CharacterLabel", actionLabel);

	actionLabel->Configure(Size(600, 140), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), nullptr);
    actionLabel->SetAlignment(Text::Alignments::LEFT);

	actionLabel->Enable();
	actionLabel->SetParent(this);

    actionLabel->Setup("No action");

    GetLeftClickEvents().Add(this, &BattleInfoPanel::HandleCharacterAction);
}

void BattleInfoPanel::HandleCharacterAction()
{
    auto actionData = battleScene->GetLastAction();
    auto string = Phrase();
    string << actionData.Character->GetName() << " rolled " << actionData.AttackRoll << " for attack. ";

    if(actionData.HasHit())
    {
        string << "Hit! ";
        string << actionData.Damage << " damage was dealt.";
    }
    else
    {
        string << "Miss!";
    }

    actionLabel->Setup(string);
}

void BattleInfoPanel::HandleUpdate()
{
}