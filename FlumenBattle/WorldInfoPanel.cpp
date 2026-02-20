#include "FlumenCore/Observer.h"

#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Sprite.hpp"
#include "FlumenEngine/Animation.h"

#include "FlumenBattle/WorldInfoPanel.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterClass.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/World/Character/Types.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Polity/HumanMind.h"

static const auto ANIMATION_NAME = "Flash";

static const auto ANIMATION_LENGTH = 1.5f;

static const Color BORDER_COLOR = Color::RED * 0.25f;

static const Color TEXT_COLOR = Color::RED * 0.5f;

static const auto SELECTED_OPACITY = Opacity(1.0f);

static const auto UNSELECTED_OPACITY = Opacity(0.5f);

static const auto MAXIMUM_CONDITIONS_PER_ITEM = 4;

static const Color WOUNDED_COLOR = Color::RED * 0.9f;

static const Color HEALING_COLOR = Color::GREEN * 0.6f;

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

    previousHitpoints = character->GetCurrentHitPoints();
}

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
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    cover = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ + 2, {this}, {false}, Opacity(1.0f)}
    );

    healthDifferenceLabel = ElementFactory::BuildText(
        {drawOrder_ + 3, {cover}}, 
        {{"Huge"}, Color::WHITE}
    );
    healthDifferenceLabel->Enable();

    animation = animator_->AddAnimation(new Animation(ANIMATION_LENGTH), ANIMATION_NAME);

    auto property = animation->AddProperty({animation, &cover->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH * 0.7f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    property = animation->AddProperty({animation, &healthDifferenceLabel->GetOpacity().Value});

    property->AddKey()->Initialize(0.0f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH * 0.5f, 1.0f);
    property->AddKey()->Initialize(ANIMATION_LENGTH, 0.0f);

    auto finishEvent = animation->GetFinishEvent();
    finishEvent->GetActions() += {cover, &Element::Disable};
}

void WorldInfoPanel::CharacterItem::HandleUpdate()
{
    auto currentHitPoints = character->GetCurrentHitPoints();

    if(currentHitPoints != previousHitpoints)
    {
        auto hitpointDifference = currentHitPoints - previousHitpoints;

        if(hitpointDifference > 0)
        {
            cover->SetSpriteColor(HEALING_COLOR);
        }
        else
        {
            cover->SetSpriteColor(WOUNDED_COLOR);
        }

        healthDifferenceLabel->Setup(ShortWord() << (hitpointDifference > 0 ? "+" : "") << hitpointDifference);

        cover->Enable();

        animation->Play();
    }

    previousHitpoints = currentHitPoints;

    auto string = Word() << currentHitPoints << "/" << character->GetMaximumHitPoints();
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
    border = ElementFactory::BuildElement <Element>
    (
        {
            size_, 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-007", true}
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    auto position = Position2(10.0f, 10.0f);

    static const auto MAXIMUM_CHARACTERS_PER_GROUP = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_CHARACTERS_PER_GROUP).Integer;
    items.Initialize(MAXIMUM_CHARACTERS_PER_GROUP);

    for(Index i = 0; i < items.GetCapacity(); ++i, position += Direction2(80.0f, 0.0f))
    {
        auto item = ElementFactory::BuildElement <CharacterItem>(
            {Size(70, 110), drawOrder_ + 2, {position, ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}, {false}, Opacity(0.8f)}
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

    auto startPosition = Position2(-140.0f, -10.0f);

    moneyCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {startPosition, this}}
    );
    auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
    moneyCounter->Setup("Coin", &playerGroup->GetMoney());
    moneyCounter->Enable();
    startPosition.x += 70.0f;

    foodCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {startPosition, this}}
    );
    foodCounter->Setup(
        "Radish", 
        std::function <int(void)> (
            [] -> int 
            {
                static const auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
                return playerGroup->GetItemAmount(world::character::ItemTypes::FOOD);
            }
        )
    );
    foodCounter->Enable();
    startPosition.x += 70.0f;

    perceptionCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {startPosition, this}}
    );
    perceptionCounter->Setup(
        "SunRays", 
        std::function <int(void)> (
            [] -> int 
            {
                static const auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
                return playerGroup->GetMostSkilledMember(world::character::SkillTypes::PERCEPTION).Bonus;
            }
        )
    );
    perceptionCounter->MakeSignSensitive();
    perceptionCounter->Enable();
    startPosition.x += 70.0f;

    weightCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {startPosition, this}}
    );
    weightCounter->Setup(
        "CrateShadowed", 
        std::function <int(void)> (
            [] -> int 
            {
                static const auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
                return playerGroup->GetCarriedWeight();
            }
        )
    );
    weightCounter->SetOffset(3.0f);
    weightCounter->Enable();
    startPosition.x += 70.0f;

    muleCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_, {startPosition, this}}
    );
    muleCounter->Setup(
        "HorseHead", 
        std::function <int(void)> (
            [] -> int 
            {
                static const auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
                return playerGroup->GetMuleCount();
            }
        )
    );
    muleCounter->SetOffset(3.0f);
    muleCounter->Enable();

    playerDomainInfoBox = ElementFactory::BuildElement <Element>(
        {Size(400, 70), drawOrder_ + 2, {Position2(0.0f, 5.0f), ElementAnchors::LOWER_CENTER, ElementPivots::MIDDLE_CENTER, this}, {false}, Opacity(0.8f)}
    );

    playerDomainInfoBorder = ElementFactory::BuildElement <Element>(
        {playerDomainInfoBox->GetSize(), drawOrder_ + 3, {playerDomainInfoBox}, {"panel-border-007", true}, Opacity(0.6f)}
    );
    playerDomainInfoBorder->SetSpriteColor(BORDER_COLOR);
    playerDomainInfoBorder->Enable();

    playerDomainInfo = ElementFactory::BuildText(
        {drawOrder_ + 3, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, playerDomainInfoBox}}, 
        {{"Small"}, TEXT_COLOR}
    );
    playerDomainInfo->Enable();

    subjectCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_ + 2, {Position2(-100.0f, -25.0f), ElementAnchors::LOWER_CENTER, ElementPivots::MIDDLE_CENTER, playerDomainInfoBox}}
    );
    subjectCounter->Setup(
        "Castle", 
        std::function <int(void)> (
            [] -> int 
            {
                static const auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
                return playerGroup->GetDomainSettlementCount();
            }
        ),
        "Medium",
        Scale2(0.55f)
    );
    subjectCounter->SetOffset(-10.0f);
    subjectCounter->Enable();

    populationCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_ + 2, {Position2(0.0f, -25.0f), ElementAnchors::LOWER_CENTER, ElementPivots::MIDDLE_CENTER, playerDomainInfoBox}}
    );
    populationCounter->Setup(
        "Houses64", 
        std::function <int(void)> (
            [] -> int 
            {
                static const auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
                return playerGroup->GetDomain()->GetPopulation();
            }
        ),
        "Medium",
        Scale2(0.6f)
    );
    populationCounter->SetOffset(-10.0f);
    populationCounter->Enable();

    industryCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
        {drawOrder_ + 2, {Position2(100.0f, -25.0f), ElementAnchors::LOWER_CENTER, ElementPivots::MIDDLE_CENTER, playerDomainInfoBox}}
    );
    industryCounter->Setup(
        "WorkHammer", 
        std::function <int(void)> (
            [] -> int 
            {
                static const auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
                return playerGroup->GetDomain()->GetIndustrialPower();
            }
        ),
        "Medium"
    );
    industryCounter->SetOffset(0.0f);
    industryCounter->Enable();

    world::WorldScene::Get()->OnPlayerBecameRuler += {this, &WorldInfoPanel::HandlePlayerBecameRuler};

    world::WorldScene::Get()->OnSceneEnabled += {this, &WorldInfoPanel::HandlePlayerBecameRuler};

    world::group::HumanMind::Get()->OnHeroJoinedParty += {this, &WorldInfoPanel::HandleHeroJoinedParty};

    world::polity::HumanMind::Get()->OnPlayerPolityDeleted += {this, &WorldInfoPanel::HandlePlayerPolityDeleted};
}

void WorldInfoPanel::HandlePlayerPolityDeleted()
{
    playerDomainInfoBox->Disable();
}

void WorldInfoPanel::HandlePlayerBecameRuler()
{
    static const auto playerGroup = world::WorldScene::Get()->GetPlayerGroup();
    if(playerGroup->GetDomain() == nullptr)
        return;

    playerDomainInfoBox->Enable();

    playerDomainInfo->Setup(Phrase() << "You rule over " << playerGroup->GetDomain()->GetRuler()->GetName());
}

void WorldInfoPanel::HandleHeroJoinedParty()
{
    RefreshCharacterList();
}

void WorldInfoPanel::RefreshCharacterList()
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
    RefreshCharacterList();
}

void WorldInfoPanel::HandleUpdate()
{
    static const auto &time = world::WorldScene::Get()->GetTime();

    const auto dayOfMonth = time.GetDayOfMonth();
    const auto monthName = time.GetMonthName();
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