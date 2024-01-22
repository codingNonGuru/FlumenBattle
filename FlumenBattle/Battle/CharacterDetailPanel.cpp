#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/Battle/CharacterDetailPanel.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/World/Character/Character.h"

using namespace battle;

static BattleController *battleController = nullptr;

static auto BORDER_COLOR = Color::RED * 0.25f;

void CharacterDetailPanel::HandleConfigure()
{
    battleController = BattleController::Get();

    auto textColor = (Color::RED * 0.3f) + (Color::BLACK * 0.7f);

    CharacterAbilityLabel::AbilityData labelData[] = {
        {world::character::AbilityTypes::STRENGTH, Word("Str")}, 
        {world::character::AbilityTypes::DEXTERITY, Word("Dex")}, 
        {world::character::AbilityTypes::CONSTITUTION, Word("Con")},
        {world::character::AbilityTypes::INTELLIGENCE, Word("Int")}, 
        {world::character::AbilityTypes::WISDOM, Word("Wis")}, 
        {world::character::AbilityTypes::CHARISMA, Word("Cha")}
        };

    auto position = Position2(-180.0f, -30.0f);
    for(Index i = 0; i < 6; ++i)
    {
        auto data = labelData[i];

        auto label = ElementFactory::BuildText<CharacterAbilityLabel>(
            {Size(150, 150), drawOrder_ + 1, {position, this}},
            {{"Medium"}, textColor}
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
        {{"Large"}, textColor}
    );
    headerLabel->Enable();

    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - Size(4, 4), 
            drawOrder_ + 1, 
            {Position2(), this}, 
            {"panel-border-031", "SlicedSprite"}
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();
}

void CharacterDetailPanel::HandleUpdate()
{
    auto character = battleController->GetSelectedCharacter();
    auto race = character->GetRace();

    Word string;
    string << race->Name << " " << character->GetClassName() << ", level " << character->level;

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