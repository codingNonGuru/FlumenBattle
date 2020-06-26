#include "FlumenCore/Observer.h"

#include "FlumenEngine/Core/Engine.hpp"

#include "FlumenBattle/PreGame/PreGameState.h"

int main()
{
	Engine::OnGameLoopStarted += []
	{ 
		pregame::PreGameState::Get()->Enter();
	};

	auto screenSize = Size(1920, 1080);
	Engine::Initialize(screenSize);

	return 0;
}
