#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/SimpleList.h"

#include "GroupTab.h"
#include "FlumenBattle/Utility/Utility.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Group/GroupDynamics.h"
#include "FlumenBattle/World/Group/GroupCore.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void GroupItem::HandleConfigure()
{
    leaderNameLabel = ElementFactory::BuildText(
        {drawOrder_, {Position2(0.0f, 5.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}},
        {{"Tiny"}, TEXT_COLOR}
    );
    leaderNameLabel->Enable();

    sizeCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> 
    (
        {drawOrder_ + 1, {Position2(-22.0f, -13.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}}
    );
    sizeCounter->Setup("Group", &groupSize, "VerySmall", Scale2(0.7f));
    sizeCounter->SetOffset(-5.0f);
    sizeCounter->Enable();

    levelCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> 
    (
        {drawOrder_ + 1, {Position2(15.0f, -13.0f), ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}}
    );
    levelCounter->Setup("Lightning", &groupLevel, "VerySmall", Scale2(0.7f));
    levelCounter->SetOffset(-5.0f);
    levelCounter->Enable();
}

void GroupItem::Setup(const group::GroupCore *group)
{
    leaderNameLabel->Setup(group->GetLeaderName());

    groupSize = group->GetSize();

    groupLevel = group->GetLevel();
}

void GroupBatchItem::HandleConfigure()
{
    mainLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(10.0f), ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}},
        {{"Small"}, TEXT_COLOR}
    );
    mainLabel->Enable();

    items.Initialize(6);

    itemList = ElementFactory::BuildSimpleList
    (
        { 
            drawOrder_, 
            {Position2{10.0f, -10.0f}, ElementAnchors::LOWER_LEFT, ElementPivots::LOWER_LEFT, this}
        },
        items.GetCapacity(),
        ListOrientations::HORIZONTAL,
        5.0f
    );
    //itemList->MakeScrollable(8, classes.size());
    itemList->Enable();

    for(int i = 0; i < items.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <GroupItem>
        (
            {Size(70, 45), drawOrder_ + 1, {itemList}, {"panel-border-015", true}, Opacity(0.5f)}
        );
        item->SetSpriteColor(BORDER_COLOR);
        item->Enable();

        *items.Add() = item;
    }
}

void GroupBatchItem::HandleUpdate()
{
    auto text = LongWord() << utility::GetClassName(groupClass) << "s: ";

    const auto settlement = WorldScene::Get()->GetPlayerSettlement();

    if(groupClass == group::GroupClasses::MERCHANT)
    {
        text << settlement->GetGroupDynamics().GetMerchantStrength() << "/" << settlement->GetGroupDynamics().GetMaximumMerchants();
    }
    else if(groupClass == group::GroupClasses::RAIDER)
    {
        text << settlement->GetGroupDynamics().GetRaiderStrength() << "/" << settlement->GetGroupDynamics().GetMaximumRaiders();
    }
    else if(groupClass == group::GroupClasses::GARRISON)
    {
        text << settlement->GetGroupDynamics().GetGarrisonStrength() << "/" << settlement->GetGroupDynamics().GetMaximumGarrisons();
    }
    else if(groupClass == group::GroupClasses::PATROL)
    {
        text << settlement->GetGroupDynamics().GetPatrolStrength() << "/" << settlement->GetGroupDynamics().GetMaximumPatrols();
    }
    else if(groupClass == group::GroupClasses::ADVENTURER)
    {
        text << settlement->GetGroupDynamics().GetAdventurerStrength() << "/" << settlement->GetGroupDynamics().GetMaximumAdventurers();
    }

    mainLabel->Setup(text);

    for(auto &item : items)
    {
        item->Disable();
    }

    auto item = items.GetStart();
    for(auto &group : settlement->GetGroupDynamics().GetGroups(groupClass))
    {
        (*item)->Setup(group);
        (*item)->Enable();

        item++;
    }
}

void GroupBatchItem::Setup(group::GroupClasses groupClass)
{
    this->groupClass = groupClass;
}

void GroupTab::HandleConfigure()
{
    auto classes = {group::GroupClasses::MERCHANT, group::GroupClasses::PATROL, group::GroupClasses::ADVENTURER, group::GroupClasses::GARRISON, group::GroupClasses::RAIDER};

    itemList = ElementFactory::BuildSimpleList
    (
        { 
            drawOrder_, 
            {Position2{0.0f, 40.0f}, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}
        },
        classes.size(),
        ListOrientations::VERTICAL,
        5.0f
    );
    //itemList->MakeScrollable(8, classes.size());
    itemList->Enable();

    batchItems.Initialize(classes.size());
    for(auto &groupClass : classes)
    {
        auto item = ElementFactory::BuildElement <GroupBatchItem>
        (
            {Size(500, 85), drawOrder_ + 1, {itemList}, {"panel-border-007", true}, Opacity(0.8f)}
        );
        item->SetSpriteColor(BORDER_COLOR);
        item->Setup(groupClass);
        item->Enable();

        *batchItems.Add() = item;
    }
}
