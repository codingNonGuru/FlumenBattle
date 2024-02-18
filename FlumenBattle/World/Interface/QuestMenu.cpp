#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "QuestMenu.h"
#include "FlumenBattle/World/Interface/QuestMenuItem.h"
#include "FlumenBattle/Config.h"
#include "FlumenBattle/World/Group/HumanMind.h"
#include "FlumenBattle/World/Group/Quest.h"

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
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}},
        {{"Large"}, TEXT_COLOR, "Quests"}
    );
    titleLabel->Enable();

    itemLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {ITEM_LIST_OFFSET, ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}
        }
    );
    itemLayout->SetDistancing(1);
    itemLayout->Enable();

    static const auto MAXIMUM_QUEST_COUNT = engine::ConfigManager::Get()->GetValue(game::ConfigValues::MAXIMUM_QUEST_COUNT).Integer;

    items.Initialize(MAXIMUM_QUEST_COUNT);

    for(auto i = 0; i < MAXIMUM_QUEST_COUNT; ++i)
    {
        auto item = items.Add();

        *item = ElementFactory::BuildElement <QuestMenuItem>(
            {Size(200, 55), drawOrder_ + 1, {itemLayout}}
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
        (*item)->Setup(&quest);

        (*item)->Enable();

        item++;
    }
}