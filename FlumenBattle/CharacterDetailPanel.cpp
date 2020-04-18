#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/TextManager.hpp"
#include "FlumenEngine/Interface/Font.hpp"
#include "FlumenEngine/Interface/FontManager.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Core/Transform.hpp"

#include "FlumenBattle/CharacterDetailPanel.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/Character.h"

BattleController *battleController = nullptr;

void CharacterDetailPanel::HandleConfigure()
{
    battleController = BattleController::Get();

    auto fontSmall = FontManager::GetFont("DominicanSmall");
    auto fontMedium = FontManager::GetFont("JSLAncientMedium");
    auto fontVerySmall = FontManager::GetFont("DominicanVerySmall");

    auto textColor = (Color::RED * 0.3f) + (Color::BLACK * 0.7f);

	strengthLabel = new Text(fontMedium, textColor);
    Interface::AddElement("CharacterLabel", strengthLabel);

	strengthLabel->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(-180.0f, 0.0f)), nullptr);

	strengthLabel->Enable();
	strengthLabel->SetParent(this);

    dexterityLabel = new Text(fontMedium, textColor);
    Interface::AddElement("CharacterLabel", dexterityLabel);

	dexterityLabel->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), nullptr);

	dexterityLabel->Enable();
	dexterityLabel->SetParent(this);

    constitutionLabel = new Text(fontMedium, textColor);
    Interface::AddElement("CharacterLabel", constitutionLabel);

	constitutionLabel->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(180.0f, 0.0f)), nullptr);

	constitutionLabel->Enable();
	constitutionLabel->SetParent(this);
}

void CharacterDetailPanel::HandleUpdate()
{
    auto character = battleController->GetSelectedCharacter();

    auto modifier = character->strength.Modifier;
    auto sign = modifier > 0 ? "+" : "";
    auto string = Word() << "Str: " << character->strength.Score << " (" << sign << modifier << ")";

    strengthLabel->Setup(string);

    modifier = character->dexterity.Modifier;
    sign = modifier > 0 ? "+" : "";
    string = Word() << "Dex: " << character->dexterity.Score << " (" << sign << modifier << ")";

    dexterityLabel->Setup(string);

    modifier = character->constitution.Modifier;
    sign = modifier > 0 ? "+" : "";
    string = Word() << "Con: " << character->constitution.Score << " (" << sign << modifier << ")";

    constitutionLabel->Setup(string);
}