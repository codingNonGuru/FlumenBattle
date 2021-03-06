#include "FlumenCore/Observer.h"

#include "FlumenEngine/Core/Engine.hpp"

#include "FlumenBattle/World/WorldState.h"

int main()
{
	Engine::OnGameLoopStarted += []
	{ 
		world::WorldState::Get()->Enter(); 
	};

	auto screenSize = Size(1920, 1080);
	Engine::Initialize(screenSize);

	return 0;
}
