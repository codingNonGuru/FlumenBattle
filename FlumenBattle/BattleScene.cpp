#include "FlumenEngine/Render/RenderManager.hpp"

#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/CharacterModel.h"
#include "FlumenBattle/GroupFactory.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/BattleTileModel.h"
#include "FlumenBattle/BattleMap.h"

BattleScene::BattleScene() {}

void BattleScene::Initialize()
{
    battleMap = new BattleMap(15);

    groups.Initialize(64);
    GroupFactory::Create(groups);
    //GroupFactory::Create(groups);

    model = new CharacterModel();
    model->Initialize();

    battleTileModel = new BattleTileModel();
    battleTileModel->Initialize();

    selected = nullptr;
}

void BattleScene::Render()
{
    battleTileModel->Render();
}

void BattleScene::SelectCharacter(Character* newlySelected)
{
    if(selected != nullptr)
    {
        selected->Deselect();
    }

    selected = newlySelected;

    if(selected != nullptr)
    {
        selected->Select();
    }
}

void BattleScene::TargetCharacter(Character* target)
{
    if(selected == nullptr)
        return;

    if(target == nullptr)
        return;

    selected->Attack(*target);
}