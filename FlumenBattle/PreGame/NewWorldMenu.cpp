#include "FlumenEngine/Interface/ElementFactory.h"
#include "FlumenEngine/Interface/Text.hpp"
#include "FlumenEngine/Core/InputHandler.hpp"

#include "NewWorldMenu.h"
#include "FlumenBattle/PreGame/PreGameState.h"

using namespace pregame;

#define DEFAULT_WORLD_SIZE (MINIMUM_WORLD_SIZE + MAXIMUM_WORLD_SIZE) / 2

void NewWorldMenu::HandleConfigure() 
{
    sizeParameterValue = 80;//DEFAULT_WORLD_SIZE;

    Phrase text = "Size: "; 
    text << sizeParameterValue;

    sizeLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, -40.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, text}
    );
    sizeLabel->Enable();

    generateLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 0.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "[G]enerate"}
    );
    generateLabel->Enable();

    backLabel = ElementFactory::BuildText(
        {Size(150, 150), drawOrder_ + 1, {Position2(0.0f, 40.0f), this}},
        {{"JSLAncient", "Large"}, Color::RED * 0.5f, "[B]ack"}
    );
    backLabel->Enable();
}

void NewWorldMenu::HandleEnable()
{
    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_G, {this, &NewWorldMenu::OnGenerateWorldPressed});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_LEFT, {this, &NewWorldMenu::OnSizeDecreasePressed});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_RIGHT, {this, &NewWorldMenu::OnSizeIncreasePressed});

    InputHandler::RegisterEvent(SDL_Scancode::SDL_SCANCODE_B, {this, &NewWorldMenu::OnBackPressed});
}

void NewWorldMenu::HandleDisable() 
{
    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_G);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_LEFT);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_RIGHT);

    InputHandler::UnregisterEvent(SDL_Scancode::SDL_SCANCODE_B);
}

void NewWorldMenu::OnGenerateWorldPressed()
{
    this->Disable();

    PreGameState::Get()->GenerateNewWorld({sizeParameterValue});
}

void NewWorldMenu::OnSizeIncreasePressed()
{
    sizeParameterValue++;
    if(sizeParameterValue > MAXIMUM_WORLD_SIZE)
    {
        sizeParameterValue = MAXIMUM_WORLD_SIZE;
    }
    
    Phrase text = "Size: "; 
    text << sizeParameterValue;

    sizeLabel->Setup(text);
}

void NewWorldMenu::OnSizeDecreasePressed()
{
    sizeParameterValue--;
    if(sizeParameterValue < MINIMUM_WORLD_SIZE)
    {
        sizeParameterValue = MINIMUM_WORLD_SIZE;
    }

    Phrase text = "Size: "; 
    text << sizeParameterValue;

    sizeLabel->Setup(text);
}

void NewWorldMenu::OnBackPressed()
{
    this->Disable();

    PreGameState::Get()->OpenNewGameMenu();
}