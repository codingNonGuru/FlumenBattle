#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "ResourceItem.h"
#include "EconomyTab.h"
#include "ResourceHoverInfo.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/Config.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto HIGHLIGHT_COLOR = Color::RED * 0.8f;

void ResourceItem::HandleConfigure()
{
    SetSpriteColor(BORDER_COLOR);

    icon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {Position2(10.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}, 
            {"Radish", false}
        }
    );
    icon->Enable();

    nameLabel = ElementFactory::BuildRichText(
        {drawOrder_ + 1, {Position2(50.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, HIGHLIGHT_COLOR, "<2>W<1>ool"}
    );
    nameLabel->Enable();

    storageBar = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(80, 28), drawOrder_ + 1, {Position2(190.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}, {"BaseBar", true}},
        {"BaseFillerRed", {6.0f, 6.0f}}
    );
    storageBar->Enable();

    storedLabel = ElementFactory::BuildText(
        {drawOrder_ + 3, {Position2(0.0f, 0.0f), storageBar}}, 
        {{"VerySmall"}, Color::WHITE}
    );
    storedLabel->Enable();

    outputLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(360.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"VerySmall"}, TEXT_COLOR}
    );
    outputLabel->Enable();

    inputLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(420.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"VerySmall"}, TEXT_COLOR}
    );
    inputLabel->Enable();

    workerLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(480.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"VerySmall"}, TEXT_COLOR}
    );
    workerLabel->Enable();

    buildingCounter = nullptr;

    buildingCount = 0;

    needBar = nullptr;
}

void ResourceItem::HandleUpdate()
{
    auto text = Word() << resource->Storage;

    storedLabel->Setup(text);

    auto midtermOutput = settlement->GetResourceHandler().GetPotentialMidtermOutput(resource->Type->Type);

    text = Word("+") << midtermOutput;

    outputLabel->Setup(text);

    text = Word("-") << settlement->GetResourceHandler().GetPotentialMidtermInput(resource->Type->Type);;

    inputLabel->Setup(text);

    text = Word() << resource->Workforce;

    workerLabel->Setup(text);

    if(needBar != nullptr)
    {
        static const auto TICKS_PER_NEED_SATISFACTION = engine::ConfigManager::Get()->GetValue(game::ConfigValues::TICKS_PER_NEED_SATISFACTION).Integer;

        auto satisfaction = settlement->GetNeedSatisfaction(resource->Type->Type);

        auto progress = (float)satisfaction / (float)TICKS_PER_NEED_SATISFACTION;

        needBar->SetProgress(progress);

        needLabel->Setup(ShortWord() << satisfaction);
    }

    auto storageRatio = (float)resource->Storage / (float)settlement->GetStorage();
    storageBar->SetProgress(storageRatio);

    if(IsHovered() == true)
    {
        SetOpacity(0.8f);

        HandleHover();
    }
    else
    {
        SetOpacity(0.5f);
    }

    if(buildingCounter != nullptr)
    {
        const auto &building = settlement->GetBuilding(resource->Type->RelatedBuilding);
        if(&building != nullptr)
        {
            buildingCount = building.GetAmount();
        }
        else
        {
            buildingCount = 0;
        }
    }
}

void ResourceItem::Setup(settlement::Resource *resource, const settlement::Settlement *settlement, EconomyTab *tab)
{
    this->resource = resource;

    this->settlement = settlement;

    this->parentTab = tab;

    auto name = Word("<2>") << resource->Type->Name.GetFirstCharacter() << "<1>" << resource->Type->Name.Get() + 1;
    nameLabel->Setup(name);

    icon->SetTexture(resource->Type->TextureName);

    bool relatesToNeed = 
        resource->Type->Type == settlement::ResourceTypes::COOKED_FOOD || 
        resource->Type->Type == settlement::ResourceTypes::FURNITURE || 
        resource->Type->Type == settlement::ResourceTypes::CLOTHING || 
        resource->Type->Type == settlement::ResourceTypes::POTTERY ||
        resource->Type->Type == settlement::ResourceTypes::FOOD;

    if(needBar == nullptr && relatesToNeed == true)
    {
        needBar = ElementFactory::BuildProgressBar <ProgressBar>(
            {Size(60, 24), drawOrder_ + 1, {Position2(-10.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_RIGHT, this}, {"BaseBar", true}},
            {"BaseFillerRed", {6.0f, 6.0f}}
        );
        needBar->Enable();

        needLabel = ElementFactory::BuildText(
            {drawOrder_ + 2, {needBar}}, 
            {{"VerySmall"}, Color::WHITE}
        );
        needLabel->Enable();
    }

    if(buildingCounter == nullptr && resource->Type->IsProductionTileBased == false && resource->Type->RelatedBuilding != settlement::BuildingTypes::NONE)
    {
        SetInteractivity(true);

        buildingCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> (
            {drawOrder_ + 1, {Position2(300.0f, -3.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}
        );
        buildingCounter->Setup(
            "Houses64", 
            &buildingCount,
            "VerySmall",
            Scale2(0.4f)
        );
        buildingCounter->SetOffset({-15.0f, 5.0f});
        buildingCounter->Enable();
    }
}

void ResourceItem::HandleLeftClick()
{
    polity::HumanMind::Get()->ChangeResourceWorkforce(resource, true);
}

void ResourceItem::HandleRightClick()
{
    polity::HumanMind::Get()->ChangeResourceWorkforce(resource, false);
}

void ResourceItem::HandleHover()
{
    if(resource == nullptr)
        return;

    auto hoverInfo = parentTab->GetHoverDevice();

    if(world::WorldScene::Get()->GetPlayerSettlement()->GetFreeWorkerCount() == 0)
    {
        hoverInfo->Disable();

        return;
    }

    hoverInfo->Setup(this);

    hoverInfo->Enable();
}