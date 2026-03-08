#include "FlumenEngine/Interface/ElementFactory.h"

#include "ConditionItem.h"
#include "FlumenBattle/World/Settlement/Condition.h"

using namespace world::interface::rule;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const auto HIGHLIGHT_COLOR = Color::RED * 0.9f;

void ConditionItem::HandleConfigure()
{
    label = ElementFactory::BuildRichText(
        {drawOrder_ + 1, {ElementAnchors::MIDDLE_RIGHT, ElementPivots::MIDDLE_LEFT, this}}, 
        {{"Small"}, TEXT_COLOR, HIGHLIGHT_COLOR, "Hunger"}
    );
    label->Enable();
}

void ConditionItem::Setup(const settlement::Condition *condition)
{
    SetTexture(condition->Type->TextureName);

    AdjustSizeToTexture();

    auto name = condition->Type->Name;

    label->Setup(Word("<2>") << name.GetFirstCharacter() << "<1>" << name.Get() + 1);
}

