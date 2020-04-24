#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/Transform.hpp"

#include "FlumenBattle/CharacterDetailPanel.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/Character.h"

struct AbilityLabelData
{
    AbilityTypes Type;

    Word Prefix;
};

static BattleController *battleController = nullptr;

void CharacterDetailPanel::HandleConfigure()
{
    battleController = BattleController::Get();

    auto textColor = (Color::RED * 0.3f) + (Color::BLACK * 0.7f);

    AbilityLabelData labelData[] = {
        {AbilityTypes::STRENGTH, Word("Str")}, {AbilityTypes::DEXTERITY, Word("Dex")}, {AbilityTypes::CONSTITUTION, Word("Con")},
        {AbilityTypes::INTELLIGENCE, Word("Int")}, {AbilityTypes::WISDOM, Word("Wis")}, {AbilityTypes::CHARISMA, Word("Cha")}
        };

    auto position = Position2(-180.0f, -30.0f);
    for(Index i = 0; i < 6; ++i)
    {
        auto data = labelData[i];

        auto label = new CharacterAbilityLabel({"JSLAncient", "Medium"}, textColor, data.Type, data.Prefix);
        label->Configure(Size(150, 150), drawOrder_ + 1, position);

        label->Enable();
        label->SetParent(this);

        position.x += 180.0f;
        if(i == 2)
        {
            position.x = -180.0f;
            position.y += 35.0f;
        }
    }

    headerLabel = new Text({"JSLAncient", "Large"}, textColor);
    headerLabel->Configure(Size(150, 150), drawOrder_ + 1, Position2(0.0f, -80.0f));

    headerLabel->Enable();
    headerLabel->SetParent(this);
}

void CharacterDetailPanel::HandleUpdate()
{
    auto character = battleController->GetSelectedCharacter();
    auto race = character->GetRace();

    Word string;
    string << race->Name << " " << character->GetName() << ", level " << character->level;

    headerLabel->Setup(string);
}

void CharacterAbilityLabel::HandleUpdate()
{
    auto character = battleController->GetSelectedCharacter();

    const auto& ability = character->GetAbility(abilityType);
    auto sign = ability.Modifier > 0 ? "+" : "";
    auto string = abilityPrefix;
    string << ": " << ability.Score << " (" << sign << ability.Modifier << ")";

    Setup(string);
}