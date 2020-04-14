#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenBattle/BattleState.h"

int main()
{
	Engine::OnGameLoopStarted.Add
	(
		[](){ BattleState::Get()->Enter(); }
	);

	auto screenSize = Size(1920, 1080);
	Engine::Initialize(screenSize);

	return 0;
}
