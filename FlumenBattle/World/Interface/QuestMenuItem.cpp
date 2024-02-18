#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "QuestMenuItem.h"
#include "FlumenBattle/World/Group/Quest.h"
#include "FlumenBattle/World/Settlement/Settlement.h"

using namespace world::interface;

static const auto TEXT_COLOR = Color::RED * 0.5f;

void QuestMenuItem::HandleConfigure()
{
    descriptionLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::UPPER_LEFT, ElementPivots::UPPER_LEFT, this}},
        {{"Medium"}, TEXT_COLOR}
    );
    descriptionLabel->Enable();

    extraInfoLabel = ElementFactory::BuildText(
        {drawOrder_ + 1, {ElementAnchors::LOWER_LEFT, ElementPivots::UPPER_LEFT, descriptionLabel}},
        {{"Small"}, TEXT_COLOR}
    );
    extraInfoLabel->Enable();
}

void QuestMenuItem::Setup(const group::Quest *newQuest)
{
    quest = newQuest;
}

void QuestMenuItem::HandleUpdate()
{
    if(quest->Type == group::QuestTypes::DELIVER_ITEM)
    {
        auto text = Phrase("Deliver item to ") << quest->Data.TargetSettlement->GetName();
        descriptionLabel->Setup(text);
    }
    else
    {
        auto text = Phrase("Defeat bandits");
        descriptionLabel->Setup(text);
    }

    auto text = Phrase("   From ") << quest->Origin->GetName() << " (" << quest->GetDaysLeft() << " days left)";
    extraInfoLabel->Setup(text);
}