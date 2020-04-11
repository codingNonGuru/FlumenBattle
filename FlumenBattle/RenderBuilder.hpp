#pragma once

class RenderBuilder
{
	static RenderBuilder* instance_;

	RenderBuilder();

	static void HandleEngineInitialized();

public:
	static void Initialize();
};
