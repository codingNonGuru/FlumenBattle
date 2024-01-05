#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Sprite.hpp"

#include "FlumenBattle/WorldInfoPanel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterClass.h"

void * WorldInfoPanel::CharacterItem::operator new(size_t size)
{
    auto &allocator = WorldInfoPanel::Get()->GetItemAllocator();
    auto item = allocator.Add();

    return item;
}

void WorldInfoPanel::CharacterItem::SetCharacter(world::character::Character *_character) 
{
    character = _character;

    icon->GetSprite()->SetTexture(character->GetAvatar());
}

static Color selectedColor = Color(1.0f, 0.7f, 0.7f, 1.0f);

static Color unselectedColor = Color(1.0f, 1.0f, 1.0f, 1.0f);

void WorldInfoPanel::CharacterItem::ToggleSelection() 
{
    isSelected = isSelected == true ? false : true;

    if(isSelected)
    {
        border->Enable();
    }
    else
    {
        border->Disable();
    }

    //GetSprite()->SetColor(isSelected ? &selectedColor : &unselectedColor);
}

void WorldInfoPanel::CharacterItem::ForceSelection()
{
    isSelected = true;

    border->Enable();

    //GetSprite()->SetColor(isSelected ? &selectedColor : &unselectedColor);
}

void WorldInfoPanel::CharacterItem::HandleConfigure()
{
    /*classLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 2, {Position2(0.0f, -15.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.75f}
    );
    classLabel->Enable();*/

    icon = ElementFactory::BuildElement<Element>(
        {Size(32, 32), drawOrder_ + 1, {Position2(0.0f, 15.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}, {"Icons_01", "Sprite"}, Opacity(1.0f)}
    );
    static SpriteDrawData drawData = {Position2(), Scale2(1.5f, 1.5f), Opacity(1.0f), DrawOrder(-2)};
    icon->GetSprite()->SetDrawData(&drawData);
    icon->Enable();

    healthLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(0.0f, 20.0f), this}}, 
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f}
    );
    healthLabel->Enable();

    conditionsLabel = ElementFactory::BuildText(
        {Size(70, 100), drawOrder_ + 1, {Position2(3.0f, 47.0f), this}}, 
        {{"JSLAncient", "VerySmall"}, Color::RED * 0.5f, "F C S"}
    );
    conditionsLabel->SetAlignment(Text::Alignments::LEFT);
    conditionsLabel->Enable();

    border = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ + 1, {Position2(0.0f, 0.0f), this}, {"panel-border-019", "SlicedSprite"}, Opacity(0.5f)}
    );
    border->Disable();
}

void WorldInfoPanel::CharacterItem::HandleUpdate()
{
    /*auto className = character->GetClass()->Name;
    classLabel->Setup(className.GetFirstCharacter());*/

    auto string = Word() << character->GetCurrentHitPoints() << "/" << character->GetMaximumHitPoints();
    healthLabel->Setup(string);

    string = Word() << (character->HasCondition(world::character::Conditions::FATIGUE) ? "F" : "");
    conditionsLabel->Setup(string);
}

void WorldInfoPanel::HandleConfigure() 
{
    auto position = Position2(10.0f, 20.0f);

    items.Initialize(16);

    for(Index i = 0; i < items.GetCapacity(); ++i, position += Direction2(80.0f, 0.0f))
    {
        auto item = ElementFactory::BuildElement <CharacterItem>(
            {Size(70, 105), drawOrder_ + 1, {position, ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}, {"panel-019", "SlicedSprite"}, Opacity(0.5f)}
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

void WorldInfoPanel::SelectCharacter(int index, bool isInInventoryMode)
{
    assert(index < items.GetSize() && "Character selection index is off the charts.\n");

    if(isInInventoryMode == false)
    {
        if(index == selectionIndex)
        {
            auto item = items.Get(selectionIndex);
            item->ToggleSelection();

            selectionIndex = -1;
        }
        else
        {
            auto item = items.Get(selectionIndex);
            item->ToggleSelection();

            item = items.Get(index);
            item->ToggleSelection();

            selectionIndex = index;
        }
    }
    else
    {
        if(index != selectionIndex)
        {
            auto item = items.Get(selectionIndex);
            item->ToggleSelection();

            item = items.Get(index);
            item->ToggleSelection();

            selectionIndex = index;
        }
    }
}

void WorldInfoPanel::HandleInventoryOpen(int index)
{
    if(index == selectionIndex)
    {
        auto item = items.Get(selectionIndex);
        item->ForceSelection();
    }
    else
    {
        auto item = items.Get(index);
        item->ToggleSelection();

        selectionIndex = index;
    }
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
    for(auto &character : characters)
    {
        item->SetCharacter(&character);
        item->Enable();
        item++;
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