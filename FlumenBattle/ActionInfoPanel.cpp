#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/TextManager.hpp"
#include "FlumenEngine/Interface/Font.hpp"
#include "FlumenEngine/Interface/FontManager.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Render/ShaderManager.hpp"
#include "FlumenEngine/Render/Shader.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/ActionInfoPanel.h"
#include "FlumenBattle/Weapon.h"
#include "FlumenBattle/Spell.h"

static BattleController * battleController = nullptr;

const Color HIGHLIGHTED_COLOR = (Color::RED * 0.5f) + (Color::BLACK * 0.5f);

const Color DEFAULT_COLOR = (Color::RED * 0.5f) + (Color::WHITE * 0.5f);

void ActionInfoPanel::HandleConfigure() 
{
    wasLayoutRefreshed = false;

    battleController = BattleController::Get();

    battleController->OnCharacterSelected.Add(this, &HandleCharacterSelected);
    battleController->OnActionSelected.Add(this, &HandleActionSelected);
    battleController->OnActionSelected.Add(this, &HandleSubactionSelected);
    battleController->OnSubactionSelected.Add(this, &HandleSubactionSelected);

    auto fontSmall = FontManager::GetFont("DominicanSmall");

    actionLabels.Initialize(8);
    for(Index i = 0; i < actionLabels.GetCapacity(); ++i)
    {
        auto label = new Text(fontSmall, DEFAULT_COLOR);
        Interface::AddElement("ActionLabel", label);

        label->Configure(Size(300, 100), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), nullptr);

        label->SetParent(this);

        *actionLabels.Allocate() = label;
    }

    auto spriteShader = ShaderManager::GetShaderMap().Get("Sprite");
    subactionLabels.Initialize(8);
    for(Index i = 0; i < subactionLabels.GetCapacity(); ++i)
    {
        auto sprite = new Sprite(nullptr, spriteShader);

        auto label = new Text(fontSmall, DEFAULT_COLOR);
        Interface::AddElement("SubactionLabel", label);

        label->Configure(Size(300, 100), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), sprite);

        label->SetParent(this);

        *subactionLabels.Allocate() = label;
    }

    actionLayoutGroup = new LayoutGroup((Array <Element *>&)actionLabels, Position3(0.0f, -20.0f, 0.0f));
    subactionLayoutGroup = new LayoutGroup((Array <Element *>&)subactionLabels, Position3(0.0f, 20.0f, 0.0f));
}

void ActionInfoPanel::HandleUpdate() 
{
    wasLayoutRefreshed = false; 
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
            case CharacterActions::ATTACK:
                string << "Attack";
                break;
            case CharacterActions::CAST_SPELL:
                string << "Cast spell";
                break;
            case CharacterActions::HELP:
                string << "Help";
                break;
            case CharacterActions::DODGE:
                string << "Dodge";
                break;
            case CharacterActions::DASH:
                string << "Dash";
                break;
            default:
                string << "Other";
                break;
        }

        label->Setup(string);

        label->AdjustSize();
    }

    HandleSubactionSelected();

    HandleActionSelected();
}

void ActionInfoPanel::HandleSubactionSelected()
{
    auto character = battleController->GetSelectedCharacter();
    if(character == nullptr)
        return;

    switch(character->GetSelectedAction()->Type)
    {
        case CharacterActions::ATTACK:
            RefreshWeapons();
            break;
        case CharacterActions::CAST_SPELL:
            RefreshSpells();
            break;
        default:
            DisableSubactionLabels();
            break;
    }
}

void ActionInfoPanel::RefreshWeapons()
{
    auto character = battleController->GetSelectedCharacter();

    const auto & weapons = character->GetWeapons();

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

        label->AdjustSize();
    }

    HandleWeaponSelected();
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

        label->AdjustSize();
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

    RefreshLayout();
}

void ActionInfoPanel::HandleWeaponSelected()
{
    auto character = battleController->GetSelectedCharacter();
    if(character == nullptr)
        return;

    auto selectedWeapon = character->GetSelectedWeapon();

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

void ActionInfoPanel::RefreshLayout()
{
    if(wasLayoutRefreshed)
        return;

    wasLayoutRefreshed = true;

    actionLayoutGroup->Refresh();

    subactionLayoutGroup->Refresh();
}