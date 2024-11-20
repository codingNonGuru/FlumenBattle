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

    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(30.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, "Wool"}
    );
    nameLabel->Enable();
}

void ResourceItem::Setup(settlement::ResourceTypes resource)
{
    resourceType = resource;

    auto type = settlement::ResourceFactory::Get()->CreateType(resource);

    nameLabel->Setup(type->Name);
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
        item->Setup(settlement::ResourceTypes(i));
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
}