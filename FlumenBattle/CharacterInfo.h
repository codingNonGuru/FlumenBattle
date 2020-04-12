#pragma once

#include "FlumenEngine/Interface/Element.hpp"

class Text;
class Camera;

class Character;
class BattleScene;

class CharacterInfo : public Element
{
    BattleScene* battleScene;

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

    friend class CharacterModel;

public:
    void Select();

    void Deselect();
};