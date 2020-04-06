#include "FlumenEngine/Core/Engine.hpp"
#include "FlumenEngine/Core/SceneManager.hpp"

#include "FlumenBattle/BattleScene.h"
#include "FlumenBattle/Types.hpp"

int main()
{
	auto screenSize = Size(1920, 1080);
	Engine::Initialize(screenSize);

	SceneManager::AddScene(new BattleScene(), Scenes::BATTLE);
	SceneManager::Get(Scenes::BATTLE)->Initialize();
	SceneManager::Get(Scenes::BATTLE)->Enable();

	Engine::StartGameLoop();

	return 0;
}
