#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"

#include "InventoryMenu.h"
#include "FlumenBattle/World/Character/Character.h"
#include "FlumenBattle/World/Character/CharacterClass.h"

using namespace world::interface;

void InventoryMenu::HandleConfigure()
{
    characterLabel = ElementFactory::BuildText(
        {Size(100, 100), drawOrder_ + 1, {Position2(0.0f, 0.0f), ElementAnchors::MIDDLE_CENTER, ElementPivots::MIDDLE_CENTER, this}}, 
        {{"JSLAncient", "Medium"}, Color::RED * 0.5f, "Cleric"}
    );
    characterLabel->Enable();
}

void InventoryMenu::SelectCharacter(character::Character *newCharacter)
{
    character = newCharacter;

    characterLabel->Setup(character->GetClass()->Name.Get());
}