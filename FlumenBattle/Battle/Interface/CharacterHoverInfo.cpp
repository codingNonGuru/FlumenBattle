#include "FlumenEngine/Utility/Color.hpp"
#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/LayoutGroup.h"

#include "CharacterHoverInfo.h"
#include "FlumenBattle/Battle/CharacterInfo.h"
#include "FlumenBattle/Battle/Combatant.h"
#include "FlumenBattle/Battle/BattleTile.h"
#include "FlumenBattle/Battle/BattleInterface.h"
#include "FlumenBattle/Types.hpp"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Interface/ResourceCounter.h"

using namespace battle::interface;

static const auto BORDER_COLOR = Color::RED * 0.25f;

static const auto TEXT_COLOR = Color::RED * 0.5f;

static const SpriteDescriptor BORDER_SPRITE = {"panel-border-007", "SlicedSprite"};

static const auto BORDER_OPACITY = Opacity(0.8f);

void CharacterHoverInfo::HandleConfigure() 
{
    border = ElementFactory::BuildElement <Element>(
        {size_, drawOrder_ + 1, {Position2(), this}, BORDER_SPRITE, BORDER_OPACITY}
    );
    border->SetSpriteColor(BORDER_COLOR);
    border->Enable();

    layout = ElementFactory::BuildElement <LayoutGroup> 
    (
        {Size(), drawOrder_, {Position2(0.0f, 10.0f), ElementAnchors::UPPER_CENTER, ElementPivots::UPPER_CENTER, this}}
    );
    layout->SetDistancing(1, 0.0f, 0.0f);
    layout->Enable();

    nameLabel = ElementFactory::BuildText(
        {Size(), drawOrder_ + 1, {Position2(), layout}}, 
        {{"JSLAncient", "Large"}, TEXT_COLOR, ""}
    );
    nameLabel->Enable();

    classLabel = ElementFactory::BuildText(
        {Size(), drawOrder_ + 1, {Position2(), layout}}, 
        {{"JSLAncient", "Medium"}, TEXT_COLOR, ""}
    );
    classLabel->Enable();

    counterGroup = ElementFactory::BuildElement <LayoutGroup> 
    (
        {Size(), drawOrder_, {Position2(), layout}}
    );
    counterGroup->LockHeight(80.0f);
    counterGroup->SetDistancing(2, 60.0f, 35.0f);
    counterGroup->Enable();

    healthCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> 
    (
        {Size(), drawOrder_ + 1, {Position2(), counterGroup}}
    );
    healthCounter->Enable();

    armorCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> 
    (
        {Size(), drawOrder_ + 1, {Position2(), counterGroup}}
    );
    armorCounter->Enable();

    attackCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> 
    (
        {Size(), drawOrder_ + 1, {Position2(), counterGroup}}
    );
    attackCounter->MakeSignSensitive();
    attackCounter->Enable();

    spellCounter = ElementFactory::BuildElement <world::interface::ResourceCounter> 
    (
        {Size(), drawOrder_ + 1, {Position2(), counterGroup}}
    );
    spellCounter->MakeSignSensitive();
    spellCounter->Enable();

    conditionLabel = ElementFactory::BuildText(
        {Size(), drawOrder_ + 1, {Position2(0.0f, 0.0f), layout}}, 
        {{"JSLAncient", "Medium"}, TEXT_COLOR, ""}
    );
    conditionLabel->LockWidth(size_.x - 20);
    conditionLabel->SetAlignment(Text::Alignments::LEFT);
    conditionLabel->Enable();
}

void CharacterHoverInfo::HandleUpdate() 
{
    characterPosition = characterInfo->GetCombatant()->GetTile()->Position;

    if(characterInfo->IsHovered() == false)
    {
        Disable();
    }

    auto &conditions = characterInfo->GetCombatant()->GetCharacter()->GetConditions();

    Phrase text;

    auto index = 0;
    for(auto &condition : conditions)
    {
        text << condition.Type->Name;

        index++;
        if(index != conditions.GetSize())
        {
            text << "\n";
        }
    }

    conditionLabel->Setup(text);

    size_.y = layout->GetSize().y + 15;

    border->GetSize().y = size_.y;
}

void CharacterHoverInfo::Setup(CharacterInfo *characterInfo)
{
    this->characterInfo = characterInfo;

    auto combatant = characterInfo->GetCombatant();

    auto character = combatant->GetCharacter();

    characterPosition = combatant->GetTile()->Position;

    FollowWorldPosition(&characterPosition, Cameras::BATTLE, Scale2(0.0f, 1.0f), Scale2(0.0f, 30.0f));

    nameLabel->Setup(character->GetName());

    classLabel->Setup(character->GetClassName());

    healthCounter->Setup("HeartFull", &character->GetCurrentHitPoints(), "Medium", Scale2(0.7f));

    armorCounter->Setup(
        "MetalArmor", 
        [] () -> int {return BattleInterface::Get()->GetHoveredCombatant()->GetCharacter()->GetArmorClass();}, 
        "Medium", 
        Scale2(1.2f)
        );

    attackCounter->Setup(
        "MetalSword", 
        [] () -> int {return BattleInterface::Get()->GetHoveredCombatant()->GetCharacter()->GetAttackRating();}, 
        "Medium", 
        Scale2(1.2f)
        );
    
    if(character->CanCastSpells() == true)
    {
        spellCounter->Setup(
            "WandT1",
            [] () -> int {return BattleInterface::Get()->GetHoveredCombatant()->GetCharacter()->GetSpellPower();},
            "Medium", 
            Scale2(0.7f)
            );
    }
    else
    {
        spellCounter->Setup(
            "WandT1",
            "-",
            "Medium", 
            Scale2(0.7f)
            );
    }
}