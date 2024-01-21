#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class Camera;

namespace battle
{
    class Combatant;
    class BattleController;
}

namespace battle
{
    class CharacterInfo : public Element
    {
        friend class BattleInterface;

        BattleController * battleController;

        Combatant* combatant;

        Text* textLabel;

        Text* selectLabel;

        Text *targetedLabel;

        Text* hitpointLabel;

        Text* deathSavingLabel;

        Camera* camera;

        void HandleEnable() override;

        void HandleConfigure() override;

        void HandleUpdate() override;

        void HandleTargeting();

        void HandleHover() override;

    public:
        void Select();

        void Deselect();

        void SetCombatant(Combatant *);

        Combatant *GetCombatant() {return combatant;}
    };
}