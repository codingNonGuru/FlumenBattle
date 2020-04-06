#pragma once

#include "FlumenEngine/Core/Scene.hpp"

class CharacterModel;
class Group;

class BattleScene : public Scene 
{
    void Initialize() override;

    void Render() override;

    CharacterModel* model;

    Array <Group> groups;

    friend class CharacterModel;
};