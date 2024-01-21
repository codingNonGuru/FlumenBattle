#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Core/SceneManager.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/BattleController.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/Battle/BattleInfoPanel.h"
#include "FlumenBattle/World/Character/Spell.h"

using namespace battle;

static BattleController * battleController = nullptr;

void BattleInfoPanel::HandleConfigure() 
{
    battleController = BattleController::Get();
    battleController->OnCharacterActed += {this, &BattleInfoPanel::HandleCharacterAction};

    auto textColor = (Color::RED * 0.3f) + (Color::BLACK * 0.7f);
    
    actionLabel = ElementFactory::BuildText(
        {Size(600, 140), DrawOrder(4), {Position2(0.0f, 0.0f), this}},
        {{"JSLAncient", "Small"}, textColor, "No action"}
    );

    actionLabel->SetAlignment(Text::Alignments::LEFT);
	actionLabel->Enable();
}

void BattleInfoPanel::HandleCharacterAction()
{
    auto actionData = battleController->GetLastAction();
    auto character = actionData.Combatant->GetCharacter();

    auto string = Phrase();
    string << character->GetClassName();

    if(actionData.ActionType == world::character::CharacterActions::DODGE)
    {
        string << " is dodging.";
    }
    else if(actionData.ActionType == world::character::CharacterActions::DASH)
    {
        string << " has dashed.";
    }
    else if(actionData.ActionType == world::character::CharacterActions::CAST_SPELL)
    {
        string << " cast " << character->GetSelectedSpell()->Name << " on " << actionData.Combatant->GetTarget()->GetCharacter()->GetClassName() << ".";
        if(character->GetSelectedSpell()->IsOffensive)
        {
            if(actionData.HasSucceeded)
            {
                string << " Hit! The victim suffers " << actionData.Damage << " damage.";
            }
            else
            {
                string << " Miss!";
            }
            
        }
    }
    else if(actionData.ActionType == world::character::CharacterActions::ATTACK)
    {
        string << " attacked " << actionData.Combatant->GetTarget()->GetCharacter()->GetClassName() << ".";

        if(actionData.HasSucceeded)
        {
            string << " Hit! The victim suffers " << actionData.Damage << " damage.";
        }
        else
        {
            string << " Miss!";
        }
    }

    std::cout<<string<<"\n";
    actionLabel->Setup(string);
}

void BattleInfoPanel::HandleUpdate()
{
}