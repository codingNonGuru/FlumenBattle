#include "FlumenEngine/Render/RenderManager.hpp"

#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/CharacterModel.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/GroupFactory.h"

void BattleScene::Initialize()
{
    groups.Initialize(64);
    GroupFactory::Create(groups);
    GroupFactory::Create(groups);

    model = new CharacterModel();
    model->Initialize();

    std::cout<<"Render\n";
}

void BattleScene::Render()
{
    //model->Render
}