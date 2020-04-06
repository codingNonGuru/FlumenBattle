#pragma once

#include "FlumenEngine/Render/Model.hpp"

class Camera;
class Light;

class BattleScene;
class Character;
class CharacterInfo;

class CharacterModel : public Model
{
    BattleScene* battleScene;

    Array <CharacterInfo*> characterInfos;

public:
    void Initialize() override;

    void Render(Camera*, Light*) override;
};