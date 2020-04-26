#include "FlumenCore/Delegate/Delegate.hpp"

#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Core/Transform.hpp"
#include "FlumenEngine/Core/SceneManager.hpp"
#include "FlumenEngine/Render/RenderManager.hpp"
#include "FlumenEngine/Render/Camera.hpp"

#include "FlumenBattle/Character.h"
#include "FlumenBattle/Combatant.h"
#include "FlumenBattle/BattleController.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/BattleInfoPanel.h"
#include "FlumenBattle/Spell.h"

static BattleController * battleController = nullptr;

void BattleInfoPanel::HandleConfigure() 
{
    battleController = BattleController::Get();
    battleController->OnCharacterActed.Add(this, &BattleInfoPanel::HandleCharacterAction);

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
    string << character->GetName();

    if(actionData.ActionType == CharacterActions::DODGE)
    {
        string << " is dodging.";
    }
    else if(actionData.ActionType == CharacterActions::DASH)
    {
        string << " has dashed.";
    }
    else if(actionData.ActionType == CharacterActions::CAST_SPELL)
    {
        string << " cast " << character->GetSelectedSpell()->Name << " on " << actionData.Combatant->GetTarget()->GetCharacter()->GetName() << ".";
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
    else if(actionData.ActionType == CharacterActions::ATTACK)
    {
        string << " attacked " << actionData.Combatant->GetTarget()->GetCharacter()->GetName() << ".";

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