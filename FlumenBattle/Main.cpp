#include "FlumenCore/Observer.h"

#include "FlumenEngine/Core/Engine.hpp"

#include "FlumenBattle/PreGame/PreGameState.h"

int main()
{
	Engine::OnGameLoopStarted += []
	{ 
		pregame::PreGameState::Get()->Enter();
	};

	Engine::Initialize();

	return 0;
}
