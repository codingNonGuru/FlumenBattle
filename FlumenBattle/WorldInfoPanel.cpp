#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"

#include "FlumenBattle/WorldInfoPanel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/Character.h"
#include "FlumenBattle/CharacterClass.h"

void * WorldInfoPanel::CharacterItem::operator new(size_t size)
{
    auto &allocator = WorldInfoPanel::Get()->GetItemAllocator();
    auto item = allocator.Add();

    return item;
}

void WorldInfoPanel::CharacterItem::HandleConfigure()
{
    classLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(0.0f, -15.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.75f}
    );
    classLabel->Enable();

    healthLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(0.0f, 25.0f), this}}, 
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f}
    );
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
    auto position = Position2(10.0f, 10.0f);

    items.Initialize(16);

    for(Index i = 0; i < items.GetCapacity(); ++i, position += Direction2(80.0f, 0.0f))
    {
        auto item = ElementFactory::BuildElement<CharacterItem>(
            {Size(70, 100), drawOrder_ + 1, {position, ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}, {"Sprite"}, opacity_}
        );
    }

    timeLabel = ElementFactory::BuildText(
        {Size(200, 80), drawOrder_ + 1, {Position2(-50.0f, -20.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_RIGHT, this}}, 
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f}
    );
    timeLabel->Enable();

    speedLabel = ElementFactory::BuildText(
        {Size(200, 80), drawOrder_ + 1, {Position2(-50.0f, 20.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_RIGHT, this}}, 
        {{"JSLAncient", "Large"}, Color::RED * 0.65f}
    );
    speedLabel->Enable();
}

void WorldInfoPanel::HandleEnable()
{
    auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
    if(!playerGroup)
        return;

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

void WorldInfoPanel::HandleUpdate()
{
    auto time = world::WorldScene::Get()->GetTime();

    auto string = Word() << "Year " << time.YearCount << ", day " << time.DayCount << ", hour " << time.HourCount;
    timeLabel->Setup(string);   

    string = [] () -> Word
    {
        if(world::WorldScene::Get()->IsTimeFlowing() == false)
        {
            return Word() << "ii";
        }
        else
        {
            return Word() << Word(">") * world::WorldScene::Get()->GetTimeSpeed();
        }
    } ();
    speedLabel->Setup(string);
}