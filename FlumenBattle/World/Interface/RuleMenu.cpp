#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "RuleMenu.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Resource.h"
#include "FlumenBattle/World/Settlement/Building.h"
#include "FlumenBattle/World/Interface/Counter.h"
#include "FlumenBattle/World/Interface/BuildingHoverInfo.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

#define MAXIMUM_BUILDING_COUNT 16

void ResourceItem::HandleConfigure()
{
    SetSpriteColor(BORDER_COLOR);

    icon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {Position2(5.0f, 0.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}, 
            {"Radish", false}
        }
    );
    icon->Enable();

    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(40.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, "Wool"}
    );
    nameLabel->Enable();

    storedLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(160.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, "100"}
    );
    storedLabel->Enable();

    outputLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(230.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, "+1"}
    );
    outputLabel->Enable();

    inputLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(300.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, "+1"}
    );
    inputLabel->Enable();
}

void ResourceItem::HandleUpdate()
{
    auto text = Word() << resource->Storage;

    storedLabel->Setup(text);

    text = Word("+") << resource->Production;

    outputLabel->Setup(text);

    text = Word() << resource->Order;

    inputLabel->Setup(text);
}

void ResourceItem::Setup(const settlement::Resource *resource)
{
    this->resource = resource;

    nameLabel->Setup(resource->Type->Name);

    icon->SetTexture(resource->Type->TextureName);
}

void BuildingItem::HandleConfigure()
{
    SetSpriteColor(BORDER_COLOR);

    icon = ElementFactory::BuildElement <Element>
    (
        { 
            drawOrder_ + 1, 
            {this}, 
            {"Radish", false}
        }
    );
    icon->SetTextureScale(Scale2(0.8f));
    icon->Enable();

    counter = ElementFactory::BuildElement <Counter>
    (
        {drawOrder_ + 1, {Position2(0.0f, 0.0f), ElementAnchors::LOWER_RIGHT, ElementPivots::LOWER_RIGHT, this}, {"WhiteDotBackdrop", false}}
    );
    counter->Setup(&buildingCount, Scale2(1.0f), "Medium");
}

void BuildingItem::HandleUpdate()
{
    if(this->building == nullptr)
    {
        counter->Disable();
        return;
    }

    buildingCount = building->GetAmount();

    if(buildingCount == 1)
    {
        counter->Disable();
    }
    else
    {
        counter->Enable();
    }
}

void BuildingItem::HandleHover()
{
    if(building == nullptr)
        return;

    auto hoverInfo = ruleMenu->GetHoverDevice();

    hoverInfo->Setup(this);

    hoverInfo->Enable();
}

void BuildingItem::Setup(const settlement::Building *building, RuleMenu *ruleMenu)
{
    this->building = building;

    this->ruleMenu = ruleMenu;

    if(building != nullptr)
    {
        icon->SetTexture(building->GetTextureName());
        icon->Enable();
    }
    else
    {
        icon->Disable();
    }
}

void RuleMenu::HandleConfigure()
{
    border = ElementFactory::BuildElement <Element>
    (
        {
            size_ - Size(4, 4), 
            drawOrder_ + 1, 
            {this}, 
            {"panel-border-031", true}
        }
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR, "Elric"}
    );
    nameLabel->Enable();

    itemLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {Position2{0.0f, -30.0f}, this}, 
            {false},
            Opacity(0.0f)
        }
    );
    itemLayout->SetDistancing(1, 0.0f, 5.0f);
    itemLayout->Enable();

    resourceItems.Initialize((int)settlement::ResourceTypes::NONE);
    for(auto i = 0; i < (int)settlement::ResourceTypes::NONE; ++i)
    {
        auto resourceType = settlement::ResourceFactory::Get()->CreateType(settlement::ResourceTypes(i));

        auto item = ElementFactory::BuildElement <ResourceItem>
        (
            {
                Size(400, 35), 
                drawOrder_ + 1, 
                {itemLayout}, 
                {"panel-border-001", true}
            }
        );
        item->Enable();

        *resourceItems.Allocate() = item;
    }

    buildingLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_,
            {Position2{0.0f, -10.0f}, ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}, 
            {this},
            Opacity(0.0f)
        }
    );
    buildingLayout->SetDistancing(MAXIMUM_BUILDING_COUNT / 2, 5.0f, 5.0f);
    buildingLayout->Enable();

    buildingItems.Initialize(MAXIMUM_BUILDING_COUNT);
    for(auto i = 0; i < buildingItems.GetCapacity(); ++i)
    {
        auto item = ElementFactory::BuildElement <BuildingItem>
        (
            {
                Size(64, 64), 
                drawOrder_ + 1, 
                {buildingLayout}, 
                {"panel-border-001", true}
            }
        );
        item->Enable();

        *buildingItems.Allocate() = item;
    }

    buildingHoverInfo = ElementFactory::BuildElement <interface::BuildingHoverInfo>
    (
        {
            GetDrawOrder() + 5,
            {ElementAnchors::MIDDLE_CENTER, ElementPivots::UPPER_CENTER, this},
            {false}
        }
    );
}

void RuleMenu::HandleUpdate()
{
    if(settlement == nullptr)
        return;

    nameLabel->Setup(settlement->GetName());

    auto item = buildingItems.GetStart();
    for(auto &building : settlement->GetBuildings())
    {
        (*item)->Setup(&building, this);

        (*item)->SetInteractivity(true);
        
        item++;
    }

    while(item != buildingItems.GetEnd())
    {
        (*item)->Setup(nullptr, nullptr);

        (*item)->SetInteractivity(false);

        item++;
    }
}

void RuleMenu::HandleEnable()
{

}

void RuleMenu::SetCurrentSettlement(settlement::Settlement *settlement) 
{
    this->settlement = settlement;

    for(int i = 0; i < (int)settlement::ResourceTypes::NONE; ++i)
    {
        auto item = *resourceItems.Get(i);

        auto resource = settlement->GetResource(settlement::ResourceTypes(i));
        if(resource == nullptr)
        {
            item->Disable();
            continue;
        }

        item->Enable();
        item->Setup(resource);
    }
}