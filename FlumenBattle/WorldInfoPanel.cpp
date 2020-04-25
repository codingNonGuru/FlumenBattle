#include "FlumenEngine/Interface/Text.hpp"

#include "FlumenBattle/WorldInfoPanel.h"
#include "FlumenBattle/WorldScene.h"
#include "FlumenBattle/Group.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/CharacterClass.h"

void * WorldInfoPanel::CharacterItem::operator new(size_t size, Array <CharacterItem> &items)
{
    auto item = items.Add();
    return item;
}

void WorldInfoPanel::CharacterItem::HandleConfigure()
{
    classLabel = new Text({"JSLAncient", "Large"}, Color::RED * 0.75f);
    classLabel->Configure(Size(100, 100), drawOrder_ + 1, Position2(0.0f, -15.0f));

    classLabel->SetParent(this);
    classLabel->Enable();

    healthLabel = new Text({"JSLAncient", "Medium"}, Color::RED * 0.5f);
    healthLabel->Configure(Size(100, 100), drawOrder_ + 1, Position2(0.0f, 25.0f));

    healthLabel->SetParent(this);
    healthLabel->Enable();
}

void WorldInfoPanel::CharacterItem::HandleUpdate()
{
    auto className = character->GetClass()->Name;
    classLabel->Setup(className.GetFirstCharacter());

    auto string = Word() << character->GetCurrentHitPoints() << "/" << character->GetMaximumHitPoints();
    healthLabel->Setup(string);
}

void WorldInfoPanel::HandleConfigure() 
{
    auto position = Position2(0.0f, 10.0f);

    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    auto &characters = playerGroup->GetCharacters();

    items.Initialize(16);

    for(Index i = 0; i < items.GetCapacity(); ++i, position += Direction2(80.0f, 0.0f))
    {
        auto item = new(items) CharacterItem();
        item->Configure(Size(70, 100), drawOrder_ + 1, position, {"Sprite"}, opacity_);

        item->SetParent(this);
    }
}

void WorldInfoPanel::HandleEnable()
{
    auto playerGroup = WorldScene::Get()->GetPlayerGroup();
    auto &characters = playerGroup->GetCharacters();

    for(auto item = items.GetStart(); item != items.GetEnd(); ++item)
    {
        item->Disable();
    }

    auto item = items.GetStart();
    for(auto character = characters.GetStart(); character != characters.GetEnd(); ++character, ++item)
    {
        item->SetCharacter(character);
        item->Enable();
    }
}