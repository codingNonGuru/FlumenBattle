#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Sprite.hpp"

#include "FlumenBattle/WorldInfoPanel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/Group.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/Config.h"

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

static const Color BORDER_COLOR = Color::RED * 0.25f;

static const Color TEXT_COLOR = Color::RED * 0.5f;

static const auto SELECTED_OPACITY = Opacity(1.0f);

static const auto UNSELECTED_OPACITY = Opacity(0.5f);

static const auto MAXIMUM_CONDITIONS_PER_ITEM = 4;

void WorldInfoPanel::CharacterItem::ToggleSelection() 
{
    isSelected = isSelected == true ? false : true;

    if(isSelected)
    {
        border->SetOpacity(SELECTED_OPACITY);
    }
    else
    {
        border->SetOpacity(UNSELECTED_OPACITY);
    }
}

void WorldInfoPanel::CharacterItem::ForceSelection()
{
    isSelected = true;

    border->SetOpacity(SELECTED_OPACITY);
}

void WorldInfoPanel::CharacterItem::HandleConfigure()
{
    icon = ElementFactory::BuildElement<Element>(
        {Size(32, 32), drawOrder_ + 1, {Position2(0.0f, 15.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}, {"Icons_01", false}}
    );
    icon->SetTextureScale(Scale2(1.5f));
    icon->Enable();

    healthLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 20.0f), this}}, 
        {{"Medium"}, TEXT_COLOR}
    );
    healthLabel->Enable();

    conditionsLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(10.0f, -5.0f), ElementAnchors::LOWER_LEFT, ElementPivots::LOWER_LEFT, this}}, 
        {{"VerySmall"}, TEXT_COLOR, "F C S"}
    );
    conditionsLabel->SetAlignment(Text::Alignments::LEFT);
    conditionsLabel->Enable();

    border = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ + 1, {this}, {"panel-border-007", true}, UNSELECTED_OPACITY}
    );
    border->GetSprite()->SetColor(&BORDER_COLOR);
    border->Enable();
}

void WorldInfoPanel::CharacterItem::HandleUpdate()
{
    auto string = Word() << character->GetCurrentHitPoints() << "/" << character->GetMaximumHitPoints();
    healthLabel->Setup(string);

    auto &conditions = character->GetConditions();
    string.Clear();

    auto index = 0;
    for(auto &condition : conditions)
    {
        string << condition.Type->Name.GetFirstCharacter();

        index++;
        if(index == MAXIMUM_CONDITIONS_PER_ITEM)
        {
            break;
        }
    }
    conditionsLabel->Setup(string);
}

void WorldInfoPanel::HandleConfigure() 
{
    auto position = Position2(10.0f, 20.0f);

    static const auto MAXIMUM_CHARACTERS_PER_GROUP = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_CHARACTERS_PER_GROUP).Integer;
    items.Initialize(MAXIMUM_CHARACTERS_PER_GROUP);

    for(Index i = 0; i < items.GetCapacity(); ++i, position += Direction2(80.0f, 0.0f))
    {
        auto item = ElementFactory::BuildElement <CharacterItem>(
            {Size(70, 110), drawOrder_ + 1, {position, ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}, {false}, Opacity(0.5f)}
        );
    }

    timeLabel = ElementFactory::BuildText(
        {Size(200, 80), drawOrder_ + 1, {Position2(-50.0f, -20.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_RIGHT, this}}, 
        {{"Medium"}, BORDER_COLOR}
    );
    timeLabel->Enable();

    speedLabel = ElementFactory::BuildText(
        {Size(200, 80), drawOrder_ + 1, {Position2(-50.0f, 20.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_RIGHT, this}}, 
        {{"Large"}, Color::RED * 0.65f}
    );
    speedLabel->Enable();

    moneyCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {this}}
    );
    auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
    moneyCounter->Setup("Coin", &playerGroup->GetMoney());
    moneyCounter->Enable();

    foodCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {Position2(70.0f, 0.0f), this}}
    );
    foodCounter->Setup(
        "Radish", 
        std::function <int(void)> (
            [] -> int 
            {
                auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();

                return playerGroup->GetItemAmount(world::character::ItemTypes::FOOD);
            }
        )
    );
    foodCounter->Enable();

    perceptionCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {Position2(140.0f, 0.0f), this}}
    );
    perceptionCounter->Setup(
        "SunRays", 
        std::function <int(void)> (
            [] -> int 
            {
                auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();

                return playerGroup->GetMostSkilledMember(world::character::SkillTypes::PERCEPTION).Bonus;
            }
        )
    );
    perceptionCounter->MakeSignSensitive();
    perceptionCounter->Enable();

    weightCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {Position2(210.0f, 0.0f), this}}
    );
    weightCounter->Setup(
        "CrateShadowed", 
        std::function <int(void)> (
            [] -> int 
            {
                auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();

                return playerGroup->GetCarriedWeight();
            }
        )
    );
    weightCounter->SetOffset(3.0f);
    weightCounter->Enable();

    muleCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {Position2(280.0f, 0.0f), this}}
    );
    muleCounter->Setup(
        "HorseHead", 
        std::function <int(void)> (
            [] -> int 
            {
                auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();

                return playerGroup->GetMuleCount();
            }
        )
    );
    muleCounter->SetOffset(3.0f);
    muleCounter->Enable();
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
    static auto &time = world::WorldScene::Get()->GetTime();

    auto dayOfMonth = time.GetDayOfMonth();
    auto monthName = time.GetMonthName();
    auto string = Phrase() << "hour " << time.HourCount << ", day " << dayOfMonth << " of month " << monthName << ", year " << time.YearCount;
    timeLabel->Setup(string);   

    string = [] () -> Phrase
    {
        if(world::WorldScene::Get()->IsTimeFlowing() == false)
        {
            return Phrase() << "ii";
        }
        else
        {
            return Phrase() << Phrase(">") * world::WorldScene::Get()->GetTimeSpeed();
        }
    } ();
    speedLabel->Setup(string);
}