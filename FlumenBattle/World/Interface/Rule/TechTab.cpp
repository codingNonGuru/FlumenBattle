#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "TechTab.h"
#include "FlumenBattle/World/Science/Types.h"
#include "FlumenBattle/World/Science/Technology.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity/Polity.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void OptionItem::HandleConfigure()
{
    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(20.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR}
    );
    nameLabel->Enable();
}

void OptionItem::Setup(const science::TechnologyType *technology, TechTab *tab)
{
    nameLabel->Setup(technology->Name);
}

void TechTab::HandleConfigure()
{
    headerLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}, 
        {{"Large"}, TEXT_COLOR}
    );
    headerLabel->Enable();

    scienceLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::LOWER_CENTER, ElementPivots::UPPER_CENTER, headerLabel}}, 
        {{"Small"}, TEXT_COLOR}
    );
    scienceLabel->Enable();

    optionLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {Position2{0.0f, -30.0f}, this}, 
            {false},
            Opacity(0.0f)
        }
    );
    optionLayout->SetDistancing(1, 5.0f);
    optionLayout->Enable();

    optionItems.Initialize((int)science::Technologies::COUNT);
    for(auto i = 0; i < (int)science::Technologies::COUNT; ++i)
    {
        auto item = ElementFactory::BuildElement <OptionItem>
        (
            {
                Size(400, 35), 
                drawOrder_ + 1, 
                {optionLayout}, 
                {"panel-border-001", true}
            }
        );
        item->SetSpriteColor(BORDER_COLOR);
        item->Enable();

        *optionItems.Allocate() = item;
    }
}

void TechTab::HandleEnable()
{
    headerLabel->Setup("Knowledge");
}

void TechTab::HandleUpdate()
{
    for(auto &item : optionItems)
        item->Disable();

    const auto playerPolity = WorldScene::Get()->GetPlayerPolity();

    auto item = optionItems.GetStart();
    for(auto i = 0; i < (int)science::Technologies::COUNT; ++i)
    {
        auto hasTechnology = playerPolity->HasDiscoveredTechnology(science::Technologies(i));
        if(hasTechnology == true)
            continue;

        auto &technology = science::TechnologyFactory::Get()->Create(science::Technologies(i));

        (*item)->Setup(&technology, this);
        (*item)->Enable();

        item++;
    }

    auto text = Phrase() << "Research speed is " << playerPolity->GetScientificPower();

    scienceLabel->Setup(text);
}