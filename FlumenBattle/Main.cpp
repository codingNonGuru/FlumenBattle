#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenBattle/WorldState.h"

int main()
{
	Engine::OnGameLoopStarted.Add
	(
		[]{ WorldState::Get()->Enter(); }
	);

	auto screenSize = Size(1920, 1080);
	Engine::Initialize(screenSize);

	return 0;
}
