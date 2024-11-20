#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "RuleMenu.h"
#include "FlumenBattle/World/Settlement/Settlement.h"
#include "FlumenBattle/World/Settlement/Resource.h"

using namespace world::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

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
            {this}, 
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
}

void RuleMenu::HandleUpdate()
{
    if(settlement == nullptr)
        return;

    nameLabel->Setup(settlement->GetName());
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