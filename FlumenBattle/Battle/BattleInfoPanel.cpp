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
        {{"Small"}, textColor, "No action"}
    );

    actionLabel->SetAlignment(Text::Alignments::LEFT);
	actionLabel->Enable();
}

void BattleInfoPanel::HandleCharacterAction()
{
    auto actionData = battleController->GetLastAction();
    auto character = actionData.Combatant->GetCharacter();

    auto string = Phrase();
    string << character->GetName() << " the " << character->GetClassName();

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
        const auto spell = character->GetSelectedSpell();
        if(actionData.IsTargetingTile == true)
        {
            string << " cast " << spell->Name << " on ground.";
        }
        else
        {
            string << " cast " << spell->Name << " on " << actionData.Combatant->GetTarget()->GetCharacter()->GetName() << ".";
        }

        if(spell->IsOffensive)
        {
            string << "\n";

            if(spell->SaveType.HasSave == true)
            {
                auto index = 0;
                for(auto &effect : *actionData.SpellEffects)
                {
                    if(index != 0)
                    {
                        string << "\n";
                    }

                    if(effect.Throw.Result.IsAnySuccess() == true)
                    {
                        string << "Save! "; 
                    }
                    else
                    {
                        string << "Fail! "; 
                    }

                    string << "(" << effect.Throw.Result.Roll + effect.Throw.Result.Modifier << " vs. " << effect.Throw.Result.DC << ")";

                    if(effect.Damage > 0)
                    {   
                        string << " - " << effect.Throw.Combatant->GetCharacter()->GetName() << " suffers " << effect.Damage << " damage.";
                    }
                    else
                    {
                        string << " - " << effect.Throw.Combatant->GetCharacter()->GetName() << " suffers no damage.";
                    }

                    index++;
                }   
            }
            else
            {
                if(actionData.HasSucceeded == true)
                {
                    string << "Hit! ";
                }
                else
                {
                    string << "Miss! ";
                }

                string << "(" << actionData.AttackRoll << " vs. " << actionData.TargetArmorClass << ")";

                if(actionData.Damage > 0)
                {
                    string << " - " << actionData.Combatant->GetTarget()->GetCharacter()->GetName() << " suffers " << actionData.Damage << " damage.";
                }
                else
                {
                    string << " - " << actionData.Combatant->GetTarget()->GetCharacter()->GetName() << " suffers no damage.";
                }
            }
            
        }
    }
    else if(actionData.ActionType == world::character::CharacterActions::ATTACK)
    {
        string << " attacked " << actionData.Combatant->GetTarget()->GetCharacter()->GetName() << ".";

        string << "\n";

        if(actionData.HasSucceeded)
        {
            string << "Hit! ";
        }
        else
        {
            string << "Miss! ";
        }

        string << "(" << actionData.AttackRoll << " vs. " << actionData.TargetArmorClass << ")";

        if(actionData.HasSucceeded)
        {
            string << " - " << actionData.Combatant->GetTarget()->GetCharacter()->GetName() << " suffers " << actionData.Damage << " damage.";
        }
    }

    std::cout<<string<<"\n";
    actionLabel->Setup(string);
}

void BattleInfoPanel::HandleUpdate()
{
}