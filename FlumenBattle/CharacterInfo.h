#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class Camera;

class Character;
class BattleController;

class CharacterInfo : public Element
{
    friend class BattleInterface;

    BattleController * battleController;

    Character* character;

    Text* textLabel;

    Text* selectLabel;

    Text* hitpointLabel;

    Text* deathSavingLabel;

    Camera* camera;

    void HandleConfigure() override;

    void HandleUpdate() override;

    void HandleTargeting();

public:
    void Select();

    void Deselect();
};