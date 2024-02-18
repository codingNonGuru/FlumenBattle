#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "QuestMenu.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Group/Quest.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto BORDER_INNER_OFFSET = Size(4, 4);

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto ITEM_LIST_OFFSET = Position2(15.0f, 70.0f);

void QuestMenu::HandleConfigure()
{
    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - BORDER_INNER_OFFSET, 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true} 
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    titleLabel = ElementFactory::BuildText(
        {Size(), drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}},
        {{"Large"}, Color::RED * 0.5f, "Quests"}
    );
    titleLabel->Enable();

    itemLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {ITEM_LIST_OFFSET, ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}
        }
    );
    itemLayout->SetDistancing(1, 10.0f);
    itemLayout->Enable();

    static const auto MAXIMUM_QUEST_COUNT = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_QUEST_COUNT).Integer;

    items.Initialize(MAXIMUM_QUEST_COUNT);

    for(auto i = 0; i < MAXIMUM_QUEST_COUNT; ++i)
    {
        auto item = items.Add();

        *item = ElementFactory::BuildText(
            {Size(), drawOrder_ + 1, {itemLayout}},
            {{"Small"}, Color::RED * 0.5f, "Aloha"}
        );
    }
}

void QuestMenu::HandleUpdate()
{
    for(auto item : items)
    {
        item->Disable();
    }

    auto &quests = group::HumanMind::Get()->GetQuests();

    auto item = items.GetStart();
    for(auto &quest : quests)
    {
        (*item)->Enable();

        if(quest.Type == group::QuestTypes::DELIVER_ITEM)
        {
            auto text = Phrase("Deliver item to ") << quest.Data.TargetSettlement->GetName() << " (" << quest.GetDaysLeft() << " days left)";
            (*item)->Setup(text);
        }
        else
        {
            auto text = Phrase("Defeat bandits");
            (*item)->Setup(text);
        }

        item++;
    }
}