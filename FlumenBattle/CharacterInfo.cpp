#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Interface/TextManager.hpp"
#include "FlumenEngine/Interface/Font.hpp"
#include "FlumenEngine/Interface/FontManager.hpp"
#include "FlumenEngine/Interface/Interface.hpp"
#include "FlumenEngine/Core/Transform.hpp"

#include "FlumenBattle/CharacterInfo.h"
#include "FlumenBattle/Character.h"

void CharacterInfo::HandleConfigure() 
{
    auto font = FontManager::GetFont("Dominican");
    auto textColor = Color(0.5f, 0.1f, 0.05f);

	textLabel = new Text(font, textColor);
    Interface::AddElement("CharacterLabel", textLabel);

	textLabel->Configure(Size(150, 150), DrawOrder(4), new Transform(Position2(0.0f, 0.0f)), nullptr);

	textLabel->Enable();
	textLabel->SetParent(this);

    switch(character->type)
    {
    case CharacterTypes::FIGHTER:
        textLabel->Setup("F");
        break;
    case CharacterTypes::RANGER:
        textLabel->Setup("R");
        break;
    case CharacterTypes::CLERIC:
        textLabel->Setup("C");
        break;
    default:
        break;
    }

    transform_->GetPosition() = Position3(character->position, 0.0f);
}
