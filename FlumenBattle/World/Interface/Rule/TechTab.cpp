#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"
#include "FlumenEngine/Interface/ProgressBar.h"

#include "TechTab.h"
#include "FlumenBattle/World/Science/Types.h"
#include "FlumenBattle/World/Science/Technology.h"
#include "FlumenBattle/World/WorldScene.h"
#include "FlumenBattle/World/Polity/Polity.h"
#include "FlumenBattle/World/Polity/HumanMind.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"

using namespace world::interface::rule;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void TechItem::HandleConfigure()
{
    iconLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {this}}, 
        {{"Medium"}, TEXT_COLOR}
    );
    iconLabel->Enable();

    //SetInteractivity(true);
}

void TechItem::Setup(const science::TechnologyType *technology, TechTab *tab)
{
    this->technology = technology;

    if(technology == nullptr)
    {
        iconLabel->Disable();
    }
    else
    {
        auto text = [&] 
        { 
            switch(technology->Type)
            {
            case science::Technologies::WOOD_WORKING:
                return "WW";
            case science::Technologies::HAND_WASHING:
                return "HW";
            case science::Technologies::MASONRY:
                return "M";
            case science::Technologies::TRAINED_SENTINELS:
                return "TS";
            }
        } ();

        iconLabel->Setup(text);
        iconLabel->Enable();
    }
}

void OptionItem::HandleConfigure()
{
    nameLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {Position2(20.0f, 2.0f), ElementAnchors::MIDDLE_LEFT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR}
    );
    nameLabel->Enable();

    counter = ElementFactory::BuildElement <ResourceCounter>
    (
        {drawOrder_ + 1, {Position2(-80.0f, 0.0f), ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_CENTER, this}}
    );

    SetInteractivity(true);
}

void OptionItem::Setup(const science::TechnologyType *technology, TechTab *tab)
{
    nameLabel->Setup(technology->Name);

    this->technology = technology;

    counter->Setup("ScienceIcon", &technology->ResearchDuration, "VerySmall", Scale2(0.7f));
    counter->SetOffset(3.0f);
    counter->Enable();
}

void OptionItem::HandleLeftClick()
{
    polity::HumanMind::Get()->SetResearchTarget(technology->Type);
}

void OptionItem::HandleUpdate()
{
    if(isHovered_)
    {
        SetOpacity(1.0f);
    }
    else
    {
        SetOpacity(0.5f);
    }
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

    discoveryProgress = ElementFactory::BuildProgressBar <ProgressBar>(
        {Size(320, 24), drawOrder_ + 1, {Position2(0.0f, 100.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}, {"BaseBar", true}},
        {"BaseFillerBlue", {6.0f, 6.0f}}
    );
    discoveryProgress->Disable();

    discoveryLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::UPPER_CENTER, ElementPivots::LOWER_CENTER, discoveryProgress}}, 
        {{"Small"}, TEXT_COLOR}
    );
    discoveryLabel->Enable();

    optionLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {Position2{0.0f, 150.0f}, ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}, 
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

    techLayout = ElementFactory::BuildElement <LayoutGroup>
    (
        { 
            drawOrder_, 
            {Position2{0.0f, -10.0f}, ElementAnchors::LOWER_CENTER, ElementPivots::LOWER_CENTER, this}, 
            {false},
            Opacity(0.0f)
        }
    );
    techLayout->SetDistancing(8, 5.0f);
    techLayout->Enable();

    techItems.Initialize((int)science::Technologies::COUNT);
    for(auto i = 0; i < (int)science::Technologies::COUNT; ++i)
    {
        auto item = ElementFactory::BuildElement <TechItem>
        (
            {
                Size(64, 64), 
                drawOrder_ + 1, 
                {techLayout}, 
                {"panel-border-007", true}
            }
        );
        item->SetSpriteColor(BORDER_COLOR);
        item->Enable();

        *techItems.Allocate() = item;
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

    const auto researchTarget = playerPolity->GetResearchTarget();

    auto item = optionItems.GetStart();
    for(auto i = 0; i < (int)science::Technologies::COUNT; ++i)
    {
        auto hasTechnology = playerPolity->HasDiscoveredTechnology(science::Technologies(i));
        if(hasTechnology == true)
            continue;

        if(researchTarget != nullptr && researchTarget->Type == science::Technologies(i))
            continue;

        auto &technology = science::TechnologyFactory::Get()->Create(science::Technologies(i));

        (*item)->Setup(&technology, this);
        (*item)->Enable();

        item++;
    }

    auto text = Phrase() << "Discovery speed is " << playerPolity->GetScientificPower();

    scienceLabel->Setup(text);

    if(researchTarget == nullptr)
    {
        discoveryProgress->Disable();
    }
    else
    {
        discoveryProgress->Enable();

        auto progress = (float)playerPolity->GetResearchProgress() / (float)researchTarget->ResearchDuration;

        discoveryProgress->SetProgress(progress);

        discoveryLabel->Setup(Word() << "Researching " << researchTarget->Name);
    }

    for(auto &item : techItems)
        item->Setup(nullptr, this);

    auto techItem = techItems.GetStart();
    for(auto i = 0; i < (int)science::Technologies::COUNT; ++i)
    {
        if(playerPolity->HasDiscoveredTechnology(science::Technologies(i)) == false)
            continue;

        auto &technology = science::TechnologyFactory::Get()->Create(science::Technologies(i));

        (*techItem)->Setup(&technology, this);

        techItem++;
    }   
}