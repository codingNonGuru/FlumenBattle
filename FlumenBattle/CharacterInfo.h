#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class Camera;

class Character;
class BattleController;

class CharacterInfo : public Element
{
    BattleController * battleController;

    Character* character;

    Text* textLabel;

    Text* selectLabel;

    Text* hitpointLabel;

    Text* deathSavingLabel;

    Camera* camera;

    void HandleConfigure() override;

    void HandleUpdate() override;

    void HandleSelection();

    void HandleTargeting();

    friend class BattleInterface;

public:
    void Select();

    void Deselect();
};