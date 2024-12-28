#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/Battle/HumanController.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/Battle/Interface/ActionInfoPanel.h"
#include "FlumenBattle/World/Character/Weapon.h"
#include "FlumenBattle/World/Character/Spell.h"
#include "FlumenBattle/World/Character/CharacterAction.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/Battle/Combatant.h"

using namespace battle::interface;

static battle::BattleController * battleController = nullptr;

static battle::HumanController * humanController = nullptr;

const Color HIGHLIGHTED_COLOR = (Color::RED * 0.5f) + (Color::BLACK * 0.5f);

const Color DEFAULT_COLOR = (Color::RED * 0.5f) + (Color::WHITE * 0.5f);

const Color TARGETING_COLOR = Color::RED;

static auto BORDER_COLOR = Color::RED * 0.25f;

void ActionInfoPanel::HandleConfigure() 
{
    humanController = HumanController::Get();

    battleController = BattleController::Get();

    battleController->OnCharacterSelected += {this, &ActionInfoPanel::HandleCharacterSelected};

    battleController->OnActionSelected += {this, &ActionInfoPanel::HandleActionSelected};

    battleController->OnActionSelected += {this, &ActionInfoPanel::HandleSubactionSelected};

    battleController->OnSubactionSelected += {this, &ActionInfoPanel::HandleSubactionSelected};

    actionLayoutGroup = ElementFactory::BuildElement <LayoutGroup> 
    (
        {Size(), drawOrder_, {Position2(0.0f, -15.0f), this}}
    );
    actionLayoutGroup->SetDistancing(10, LABEL_DISTANCE);
    actionLayoutGroup->Enable();

    subactionLayoutGroup = ElementFactory::BuildElement <LayoutGroup> 
    (
        {Size(), drawOrder_, {Position2(0.0f, 15.0f), this}}
    );
    subactionLayoutGroup->SetDistancing(10, LABEL_DISTANCE);
    subactionLayoutGroup->Enable();

    actionLabels.Initialize(8);
    for(Index i = 0; i < actionLabels.GetCapacity(); ++i)
    {
        auto label = ElementFactory::BuildText(
            {Size(), drawOrder_ + 1, {Position2(), actionLayoutGroup}},
            {{"Medium"}, DEFAULT_COLOR}
        );

        *actionLabels.Allocate() = label;
    }

    subactionLabels.Initialize(8);
    for(Index i = 0; i < subactionLabels.GetCapacity(); ++i)
    {
        auto label = ElementFactory::BuildText(
            {Size(), drawOrder_ + 1, {Position2(), subactionLayoutGroup}},
            {{"Medium"}, DEFAULT_COLOR}
        );

        *subactionLabels.Allocate() = label;
    }

    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - Size(4, 4), 
            drawOrder_ + 1, 
            {Position2(), this}, 
            {"panel-border-031", true}
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    actionCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> 
    (
        {Size(), drawOrder_ + 1, {Position2(-130.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_CENTER, this}}
    );
    actionCounter->Enable();

    movementCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> 
    (
        {Size(), drawOrder_ + 1, {Position2(-70.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_CENTER, this}}
    );
    movementCounter->Enable();
}

void ActionInfoPanel::HandleUpdate() 
{
    auto character = battleController->GetSelectedCharacter();
    for(Index i = 0; i < subactionLabels.GetSize(); ++i)
    {
        auto label = *subactionLabels.Get(i);
        if(i == character->GetSelectedSubactionIndex())
        {
            if(humanController->IsInitiatingTargeting())
            {
                label->SetColor(TARGETING_COLOR);
            }
            else
            {
                label->SetColor(HIGHLIGHTED_COLOR);
            }
        }
    }
}

void ActionInfoPanel::HandleCharacterSelected()
{
    auto character = battleController->GetSelectedCharacter();
    if(character == nullptr)
        return;

    const auto& actions = character->GetActions();

    auto action = actions.GetStart();
    Index labelIndex = 0;
    for(auto labelIterator = actionLabels.GetStart(); labelIterator != actionLabels.GetEnd(); ++labelIterator, ++labelIndex, ++action)
    {
        auto label = *labelIterator;
        if(labelIndex >= actions.GetSize())
        {
            label->Disable();
            continue;
        }
        
        label->Enable();

        auto string = Phrase();
        string << labelIndex + 1 << " - ";

        switch(action->Type)
        {
            case world::character::CharacterActions::ATTACK:
                string << "Attack";
                break;
            case world::character::CharacterActions::CAST_SPELL:
                string << "Cast spell";
                break;
            case world::character::CharacterActions::HELP:
                string << "Help";
                break;
            case world::character::CharacterActions::DODGE:
                string << "Dodge";
                break;
            case world::character::CharacterActions::DASH:
                string << "Dash";
                break;
            default:
                string << "Other";
                break;
        }

        label->Setup(string);
    }

    HandleSubactionSelected();

    HandleActionSelected();

    auto combatant = battleController->GetSelectedCombatant();

    actionCounter->Setup("ActionFist", &combatant->GetRemainingActionCount(), "Medium");

    movementCounter->Setup("LeatherBoot", &combatant->GetMovement(), "Medium");
}

void ActionInfoPanel::HandleSubactionSelected()
{
    auto character = battleController->GetSelectedCharacter();
    if(character == nullptr)
        return;

    DisableSubactionLabels();

    switch(character->GetSelectedAction()->Type)
    {
        case world::character::CharacterActions::ATTACK:
            RefreshWeapons();
            break;
        case world::character::CharacterActions::CAST_SPELL:
            RefreshSpells();
            break;
        default:
            //DisableSubactionLabels();
            break;
    }
}

void ActionInfoPanel::RefreshWeapons()
{
    auto character = battleController->GetSelectedCharacter();

    /*const auto &weapons = character->GetWeapons();

    auto weaponIterator = weapons.GetStart();
    Index labelIndex = 0;
    for(auto labelIterator = subactionLabels.GetStart(); labelIterator != subactionLabels.GetEnd(); ++labelIterator, ++labelIndex, ++weaponIterator)
    {
        auto label = *labelIterator;
        if(labelIndex >= weapons.GetSize())
        {
            label->Disable();
            continue;
        }
        
        label->Enable();

        auto string = Phrase();
        string << labelIndex + 1 << " - " << weaponIterator->Name;
        label->Setup(string);
    }

    HandleWeaponSelected();*/
}

void ActionInfoPanel::RefreshSpells()
{
    auto character = battleController->GetSelectedCharacter();

    const auto & spells = character->GetSpells();

    auto spellIterator = spells.GetStart();
    Index labelIndex = 0;
    for(auto labelIterator = subactionLabels.GetStart(); labelIterator != subactionLabels.GetEnd(); ++labelIterator, ++labelIndex, ++spellIterator)
    {
        auto label = *labelIterator;
        if(labelIndex >= spells.GetSize())
        {
            label->Disable();
            continue;
        }
        
        label->Enable();

        auto string = Phrase();
        string << labelIndex + 1 << " - ";
        string << spellIterator->Name;

        label->Setup(string);
    }

    HandleSpellSelected();
}

void ActionInfoPanel::DisableSubactionLabels()
{
    for(auto labelIterator = subactionLabels.GetStart(); labelIterator != subactionLabels.GetEnd(); ++labelIterator)
    {
        auto label = *labelIterator;
        label->Disable();
    }
}

void ActionInfoPanel::HandleActionSelected()
{
    auto character = battleController->GetSelectedCharacter();
    if(character == nullptr)
        return;

    auto selectedAction = character->GetSelectedAction();

    const auto& actions = character->GetActions();

    auto action = actions.GetStart();
    Index labelIndex = 0;
    for(auto labelIterator = actionLabels.GetStart(); labelIterator != actionLabels.GetEnd(); ++labelIterator, ++labelIndex, ++action)
    {
        auto label = *labelIterator;
        if(labelIndex >= actions.GetSize())
        {
            break;
        }
        
        if(action == selectedAction)
        {
            label->SetColor(HIGHLIGHTED_COLOR);
        }
        else
        {
            label->SetColor(DEFAULT_COLOR);
        }
    }
}

void ActionInfoPanel::HandleWeaponSelected()
{
    auto character = battleController->GetSelectedCharacter();
    if(character == nullptr)
        return;

    /*auto selectedWeapon = character->GetSelectedWeapon();

    const auto& weapons = character->GetWeapons();

    auto weaponIterator = weapons.GetStart();
    Index labelIndex = 0;
    for(auto labelIterator = subactionLabels.GetStart(); labelIterator != subactionLabels.GetEnd(); ++labelIterator, ++labelIndex, ++weaponIterator)
    {
        auto label = *labelIterator;
        if(labelIndex >= weapons.GetSize())
        {
            break;
        }
        
        if(weaponIterator == selectedWeapon)
        {
            label->SetColor(HIGHLIGHTED_COLOR);
        }
        else
        {
            label->SetColor(DEFAULT_COLOR);
        }
    }

    RefreshLayout();*/
}

void ActionInfoPanel::HandleSpellSelected()
{
    auto character = battleController->GetSelectedCharacter();
    if(character == nullptr)
        return;

    auto selectedSpell = character->GetSelectedSpell();

    const auto& spells = character->GetSpells();

    auto spellIterator = spells.GetStart();
    Index labelIndex = 0;
    for(auto labelIterator = subactionLabels.GetStart(); labelIterator != subactionLabels.GetEnd(); ++labelIterator, ++labelIndex, ++spellIterator)
    {
        auto label = *labelIterator;
        if(labelIndex >= spells.GetSize())
        {
            break;
        }
        
        if(spellIterator == selectedSpell)
        {
            label->SetColor(HIGHLIGHTED_COLOR);
        }
        else
        {
            label->SetColor(DEFAULT_COLOR);
        }
    }
}