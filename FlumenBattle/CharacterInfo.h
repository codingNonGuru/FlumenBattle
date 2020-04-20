#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class Camera;

class Combatant;
class BattleController;

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

    void HandleConfigure() override;

    void HandleUpdate() override;

    void HandleTargeting();

public:
    void Select();

    void Deselect();

    void SetCombatant(Combatant *);
};