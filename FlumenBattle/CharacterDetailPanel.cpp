#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/CharacterDetailPanel.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/Character.h"

static BattleController *battleController = nullptr;

void CharacterDetailPanel::HandleConfigure()
{
    battleController = BattleController::Get();

    auto textColor = (Color::RED * 0.3f) + (Color::BLACK * 0.7f);

    CharacterAbilityLabel::AbilityData labelData[] = {
        {AbilityTypes::STRENGTH, Word("Str")}, {AbilityTypes::DEXTERITY, Word("Dex")}, {AbilityTypes::CONSTITUTION, Word("Con")},
        {AbilityTypes::INTELLIGENCE, Word("Int")}, {AbilityTypes::WISDOM, Word("Wis")}, {AbilityTypes::CHARISMA, Word("Cha")}
        };

    auto position = Position2(-180.0f, -30.0f);
    for(Index i = 0; i < 6; ++i)
    {
        auto data = labelData[i];

        auto label = ElementFactory::BuildText<CharacterAbilityLabel>(
            {Size(150, 150), drawOrder_ + 1, {position, this}},
            {{"JSLAncient", "Medium"}, textColor}
        );
        label->SetAbilityData(data);

        label->Enable();

        position.x += 180.0f;
        if(i == 2)
        {
            position.x = -180.0f;
            position.y += 35.0f;
        }
    }

    headerLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -80.0f), this}},
        {{"JSLAncient", "Large"}, textColor}
    );
    headerLabel->Enable();
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

    const auto& ability = character->GetAbility(abilityData.Type);
    auto sign = ability.Modifier > 0 ? "+" : "";
    auto string = abilityData.Prefix;
    string << ": " << ability.Score << " (" << sign << ability.Modifier << ")";

    Setup(string);
}