#include "IntroState.h"
#include "FlumenBattle/PreGame/PreGameState.h"

/**
 * @file IntroState.cpp
 * @brief Implements the introductory state of the game logic, 
 * transitioning to the pre-game phase upon entry.
 */

namespace intro {

/**
 * @brief Called when entering the IntroState.
 * 
 * This function initiates the transition to the PreGameState. The actual
 * business logic for entering the pre-game portion is deferred to the
 * PreGameState class.
 */
void IntroState::HandleEnter() {
    // Transition immediately to PreGameState
    pregame::PreGameState::Get()->Enter();
}

/**
 * @brief Called when exiting the IntroState.
 * 
 * Currently, no additional cleanup is required for exiting this state.
 */
void IntroState::HandleExit() {
    // No specialized logic on exit
}

} // namespace intro
